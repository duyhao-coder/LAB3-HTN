#include "tinyml.h"
float glob_temperature = 0;
float glob_humidity = 0;
void generateRandomData() {
    // Tạo ngẫu nhiên giá trị nhiệt độ và độ ẩm trong khoảng từ 0.1 đến 100
    glob_temperature = random(1, 10001) / 100.0; // random(1, 10001) chia cho 100 để có giá trị từ 0.01 đến 100
    glob_humidity = random(1, 10001) / 100.0;    // random(1, 10001) chia cho 100 để có giá trị từ 0.01 đến 100

    // In ra giá trị nhiệt độ và độ ẩm đã được tạo ngẫu nhiên để kiểm tra
    Serial.print("Generated Temperature: ");
    Serial.println(glob_temperature);  // In nhiệt độ ngẫu nhiên

    Serial.print("Generated Humidity: ");
    Serial.println(glob_humidity);     // In độ ẩm ngẫu nhiên
}

// Globals, for the convenience of one-shot setup.
namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;
    constexpr int kTensorArenaSize = 8 * 1024; // Adjust size based on your model
    uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setupTinyML()
{
    Serial.println("TensorFlow Lite Init....");
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(dht_anomaly_model_tflite); // g_model_data is from model_data.h
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        error_reporter->Report("Model provided is schema version %d, not equal to supported version %d.",
                               model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk)
    {
        error_reporter->Report("AllocateTensors() failed");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    Serial.println("TensorFlow Lite Micro initialized on ESP32.");
}
void tiny_ml_task(void *pvParameters)
{
    setupTinyML();

    while (1)
    {
        // Sinh dữ liệu ngẫu nhiên
        generateRandomData();

        // Chuẩn bị dữ liệu đầu vào
        input->data.f[0] = glob_temperature;
        input->data.f[1] = glob_humidity;

        // Thực hiện suy luận
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk)
        {
            error_reporter->Report("Invoke failed");
            return;
        }

        // Lấy kết quả đầu ra
        float result = output->data.f[0];
        Serial.print("Inference result: ");
        Serial.println(result);

        // Kiểm tra kết quả: Nếu result lớn hơn 0.5, coi là bất thường (hoặc một ngưỡng tùy chỉnh)
        if (result > 0.5)
        {
            Serial.println("Anomaly detected");
        }
        else
        {
            Serial.println("Normal data");
        }

        vTaskDelay(5000); // Đợi 5 giây trước khi thực hiện lại
    }
}
