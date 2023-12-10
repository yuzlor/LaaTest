#pragma once

#include <functional>
#include <ostream>
#include <unordered_map>

#include <meojson/json.hpp>

namespace asst
{
    enum class AsstMsg
    {
        /* Global Info */
        InternalError = 0, // �ڲ�����
        InitFailed,        // ��ʼ��ʧ��
        ConnectionInfo,    // ������ش���
        AllTasksCompleted, // ȫ���������
        AsyncCallInfo,     // �ⲿ�첽������Ϣ
        /* TaskChain Info */
        TaskChainError = 10000, // ������ִ��/ʶ�����
        TaskChainStart,         // ��������ʼ
        TaskChainCompleted,     // ���������
        TaskChainExtraInfo,     // ������������Ϣ
        TaskChainStopped,       // ������ֹͣ���ֶ�ֹͣ��
        /* SubTask Info */
        SubTaskError = 20000, // ԭ������ִ��/ʶ�����
        SubTaskStart,         // ԭ������ʼ
        SubTaskCompleted,     // ԭ���������
        SubTaskExtraInfo,     // ԭ�����������Ϣ
        SubTaskStopped,       // ԭ������ֹͣ���ֶ�ֹͣ��
    };

    inline std::ostream& operator<<(std::ostream& os, const AsstMsg& type)
    {
        static const std::unordered_map<AsstMsg, std::string> _type_name = {
            /* Global Info */
            { AsstMsg::InternalError, "InternalError" },
            { AsstMsg::InitFailed, "InitFailed" },
            { AsstMsg::ConnectionInfo, "ConnectionInfo" },
            { AsstMsg::AllTasksCompleted, "AllTasksCompleted" },
            { AsstMsg::AsyncCallInfo, "AsyncCallInfo" },
            /* TaskChain Info */
            { AsstMsg::TaskChainError, "TaskChainError" },
            { AsstMsg::TaskChainStart, "TaskChainStart" },
            { AsstMsg::TaskChainCompleted, "TaskChainCompleted" },
            { AsstMsg::TaskChainExtraInfo, "TaskChainExtraInfo" },
            { AsstMsg::TaskChainStopped, "TaskChainStopped" },
            /* SubTask Info */
            { AsstMsg::SubTaskError, "SubTaskError" },
            { AsstMsg::SubTaskStart, "SubTaskStart" },
            { AsstMsg::SubTaskCompleted, "SubTaskCompleted" },
            { AsstMsg::SubTaskExtraInfo, "SubTaskExtraInfo" },
            { AsstMsg::SubTaskStopped, "SubTaskStopped" },
        };
        return os << _type_name.at(type);
    }

    // ����Ļص��ӿ�
    using AsstMsgId = int32_t;
    using ApiCallback = void (*)(AsstMsgId msg, const char* details_json, void* custom_arg);

    // �ڲ�ʹ�õĻص�
    class Assistant;
    using AsstCallback = std::function<void(AsstMsg msg, const json::value& details, Assistant* inst)>;
}
