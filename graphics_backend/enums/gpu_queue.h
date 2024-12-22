#ifndef GPU_QUEUE_H
#define GPU_QUEUE_H

enum class GPUQueue
{
    RENDER,
    COPY,

    MAX
};

constexpr int k_RenderGPUQueueIndex = static_cast<int>(GPUQueue::RENDER);
constexpr int k_CopyGPUQueueIndex = static_cast<int>(GPUQueue::COPY);
constexpr int k_MaxGPUQueuesCount = static_cast<int>(GPUQueue::MAX);

#endif //GPU_QUEUE_H
