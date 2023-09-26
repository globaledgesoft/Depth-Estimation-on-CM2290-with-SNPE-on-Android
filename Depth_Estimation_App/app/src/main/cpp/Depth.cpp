#include "Depth.h"
#include <unistd.h>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <string>
#include <cstdlib>
#include <mutex>
#include <glob.h>
#include <dirent.h>
#include <stdio.h>


Depth::Depth()
    : m_camera_ready(false), m_image(nullptr), m_image_reader(nullptr), m_native_camera(nullptr){}

Depth::~Depth(){
    JNIEnv *env;
    java_vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    env->DeleteGlobalRef(calling_activity_obj);
    calling_activity_obj = nullptr;

    // ACameraCaptureSession_stopRepeating(m_capture_session);
    if (m_native_camera != nullptr) {
        delete m_native_camera;
        m_native_camera = nullptr;
    }

    // make sure we don't leak native windows
    if (m_native_window != nullptr) {
        ANativeWindow_release(m_native_window);
        m_native_window = nullptr;
    }

    if (m_image_reader != nullptr) {
        delete (m_image_reader);
        m_image_reader = nullptr;
    }
}

void Depth::OnCreate() {

    qc = new Qcsnpe(model_path, 2, output_layers);

}

void Depth::OnPause() {}
void Depth::OnDestroy() {}

void Depth::SetNativeWindow(ANativeWindow* native_window) {
    // Save native window
    m_native_window = native_window;
}

void Depth::SetUpCamera() {

    m_native_camera = new Native_Camera(m_selected_camera_type);
    m_native_camera->MatchCaptureSizeRequest(&m_view,
                                             ANativeWindow_getWidth(m_native_window),
                                             ANativeWindow_getHeight(m_native_window));

    LOGI("______________mview %d\t %d\n", m_view.width, m_view.height);
    LOGI("______________mview %d\t %d\n", ANativeWindow_getWidth(m_native_window),ANativeWindow_getHeight(m_native_window));
    ASSERT(m_view.width && m_view.height, "Could not find supportable resolution");

    ANativeWindow_setBuffersGeometry(m_native_window, m_view.width, m_view.height,
                                     WINDOW_FORMAT_RGBX_8888);
    m_image_reader = new Image_Reader(&m_view, AIMAGE_FORMAT_YUV_420_888);
    m_image_reader->SetPresentRotation(m_native_camera->GetOrientation());
    ANativeWindow* image_reader_window = m_image_reader->GetNativeWindow();
    m_camera_ready = m_native_camera->CreateCaptureSession(image_reader_window);
}

void Depth::CameraLoop() {
    bool buffer_printout = false;
    video_writer.open("/storage/emulated/0/appData/models/depth_video.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, cv::Size(640, 480), true);

    while (1) {
        if (m_camera_thread_stopped) { break; }
        if (!m_camera_ready || !m_image_reader) { continue; }
        //reading the image from ndk reader
        m_image = m_image_reader->GetNextImage();
        if (m_image == nullptr) { continue; }

        ANativeWindow_acquire(m_native_window);
        ANativeWindow_Buffer buffer;
        if (ANativeWindow_lock(m_native_window, &buffer, nullptr) < 0) {
            m_image_reader->DeleteImage(m_image);
            m_image = nullptr;
            continue;
        }
        if (false == buffer_printout) {
            buffer_printout = true;
            LOGI("/// H-W-S-F: %d, %d, %d, %d", buffer.height, buffer.width, buffer.stride,
                 buffer.format);
        }

        //display the image
        m_image_reader->DisplayImage(&buffer, m_image);

        //converting the ndk image into opencv format
        img_mat = cv::Mat(buffer.height, buffer.stride, CV_8UC4, buffer.bits);

        bgr_img = cv::Mat(img_mat.rows, img_mat.cols, CV_8UC3);
        cv::cvtColor(img_mat, bgr_img, cv::COLOR_RGBA2BGR);

        cv::cvtColor(bgr_img, rgb_img, cv::COLOR_BGR2RGB);

        cv::Mat res_img = cv::Mat(640, 192, CV_8UC3);
        cv::resize(bgr_img, res_img, cv::Size(640, 192));

        pred_out = qc->predict(res_img);

        auto &depth_map = pred_out["model/tf.identity/Identity:0"];
        cv::Mat mat_out(192, 640, CV_32FC1, depth_map.data());

        cv::Mat norm_depth_map = 255 * (mat_out - depth_min) / (depth_max - depth_min);
        cv::threshold(norm_depth_map, thresholded_map, 0, 0, cv::THRESH_TOZERO);
        cv::convertScaleAbs(thresholded_map, color_depth, 1);
        cv::applyColorMap(color_depth, apply_color, cv::COLORMAP_JET);
        cv::resize(apply_color, out_img, cv::Size(img_mat.rows, img_mat.cols));
        cv::imwrite("/storage/emulated/0/appData/models/depth_image.jpg",out_img);
        cv::resize(out_img, out_img, cv::Size(640, 480));
        video_writer.write(out_img);

        pred_out.clear();
        ANativeWindow_unlockAndPost(m_native_window);
        ANativeWindow_release(m_native_window);
    }
    video_writer.release();

}

