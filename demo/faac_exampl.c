#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "faac.h"

int main()
{
    // 音频参数
    unsigned long sampleRate = 8000; // 采样率
    unsigned int numChannels = 1;     // 声道数（立体声）
    unsigned int bitsPerSample = 16;  // 位深度
    unsigned long bitRate = 128000;   // 比特率 128kbps

    // FAAC 编码器句柄
    faacEncHandle hEncoder;
    faacEncConfigurationPtr config;

    // 获取编码器配置参数
    unsigned long samplesInput, maxBytesOutput;

    // 1. 打开编码器
    hEncoder = faacEncOpen(sampleRate, numChannels, &samplesInput, &maxBytesOutput);
    if (!hEncoder)
    {
        fprintf(stderr, "Failed to open FAAC encoder\n");
        return -1;
    }

    printf("FAAC encoder opened successfully\n");
    printf("Samples input: %lu\n", samplesInput);
    printf("Max bytes output: %lu\n", maxBytesOutput);

    // 2. 获取当前配置
    config = faacEncGetCurrentConfiguration(hEncoder);
    if (!config)
    {
        fprintf(stderr, "Failed to get FAAC configuration\n");
        faacEncClose(hEncoder);
        return -1;
    }

    // 3. 设置编码参数
    config->mpegVersion = MPEG2;             // MPEG版本
    config->aacObjectType = LOW;             // AAC对象类型 (LC)
    config->allowMidside = 1;                // 允许中/侧声道编码
    config->useLfe = 0;                      // 不使用低频效果声道
    config->useTns = 1;                      // 使用时间噪声整形
    config->bitRate = bitRate / numChannels; // 每声道比特率
    config->bandWidth = 0;                   // 自动选择带宽
    config->quantqual = 100;                 // 量化质量
    config->outputFormat = 1;                // 输出ADTS格式
    config->inputFormat = FAAC_INPUT_16BIT;  // 输入16位PCM

    // 4. 设置配置
    if (!faacEncSetConfiguration(hEncoder, config))
    {
        fprintf(stderr, "Failed to set FAAC configuration\n");
        faacEncClose(hEncoder);
        return -1;
    }

    // 5. 分配缓冲区
    int16_t *inputBuffer = (int16_t *)malloc(samplesInput * sizeof(int16_t));
    uint8_t *outputBuffer = (uint8_t *)malloc(maxBytesOutput);

    if (!inputBuffer || !outputBuffer)
    {
        fprintf(stderr, "Failed to allocate buffers\n");
        free(inputBuffer);
        free(outputBuffer);
        faacEncClose(hEncoder);
        return -1;
    }

    // 6. 编码循环示例
    FILE *inputFile = fopen("input.pcm", "rb");   // 输入PCM文件
    FILE *outputFile = fopen("output.aac", "wb"); // 输出AAC文件

    if (!inputFile || !outputFile)
    {
        fprintf(stderr, "Failed to open files\n");
        free(inputBuffer);
        free(outputBuffer);
        faacEncClose(hEncoder);
        return -1;
    }

    size_t samplesRead;
    int bytesWritten;

    printf("Starting encoding...\n");

    while ((samplesRead = fread(inputBuffer, sizeof(int16_t), samplesInput, inputFile)) > 0)
    {
        // 如果读取的样本数不足，用0填充
        if (samplesRead < samplesInput)
        {
            memset(&inputBuffer[samplesRead], 0, (samplesInput - samplesRead) * sizeof(int16_t));
        }

        // 执行编码
        bytesWritten = faacEncEncode(hEncoder,
                                     (int32_t *)inputBuffer, // 输入PCM数据
                                     samplesRead,            // 输入样本数
                                     outputBuffer,           // 输出AAC数据
                                     maxBytesOutput);        // 输出缓冲区大小

        if (bytesWritten < 0)
        {
            fprintf(stderr, "Encoding error\n");
            break;
        }

        // 写入编码后的数据
        if (bytesWritten > 0)
        {
            fwrite(outputBuffer, 1, bytesWritten, outputFile);
            printf("Encoded %d bytes\n", bytesWritten);
        }

        // 如果读取的样本数不足，说明文件结束
        if (samplesRead < samplesInput)
        {
            break;
        }
    }

    // 7. 刷新编码器缓冲区
    printf("Flushing encoder...\n");
    do
    {
        bytesWritten = faacEncEncode(hEncoder, NULL, 0, outputBuffer, maxBytesOutput);
        if (bytesWritten > 0)
        {
            fwrite(outputBuffer, 1, bytesWritten, outputFile);
            printf("Flushed %d bytes\n", bytesWritten);
        }
    } while (bytesWritten > 0);

    // 8. 清理资源
    fclose(inputFile);
    fclose(outputFile);
    free(inputBuffer);
    free(outputBuffer);
    faacEncClose(hEncoder);

    printf("Encoding completed successfully!\n");
    return 0;
}
