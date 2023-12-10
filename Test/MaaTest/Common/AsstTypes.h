#pragma once

#include <climits>
#include <cmath>
#include <functional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Utils/StringMisc.hpp"

#ifndef NOMINMAX
#define NOMINMAX
#endif

namespace asst
{
    constexpr double DoubleDiff = 1e-12;

    static constexpr int WindowWidthDefault = 1280;
    static constexpr int WindowHeightDefault = 720;

    static constexpr double TemplThresholdDefault = 0.8;

    enum class StaticOptionKey
    {
        Invalid = 0,
        CpuOCR = 1, // use CPU to OCR, no value. It does not support switching after the resource is loaded.
        GpuOCR = 2, // use GPU to OCR, value is gpu_id int to string. It does not support switching after the resource
        // is loaded.
    };

    enum class InstanceOptionKey
    {
        Invalid = 0,
        /* Deprecated */         // MinitouchEnabled = 1,
        TouchMode = 2,           // ����ģʽ���ã� "minitouch" | "maatouch" | "adb"
        DeploymentWithPause = 3, // �Զ�ս������롢��ȫ �Ƿ�ʹ�� ��ͣ�¸�Ա�� "0" | "1"
        AdbLiteEnabled = 4,      // �Ƿ�ʹ�� AdbLite�� "0" | "1"
        KillAdbOnExit = 5,       // �˳�ʱ�Ƿ�ɱ�� Adb ���̣� "0" | "1"
    };

    enum class TouchMode
    {
        Adb = 0,
        Minitouch = 1,
        Maatouch = 2,
        MacPlayTools = 3,
    };

    namespace ControlFeat
    {
        using Feat = int64_t;
        constexpr Feat NONE = 0;
        constexpr Feat SWIPE_WITH_PAUSE = 1 << 0;
        constexpr Feat PRECISE_SWIPE = 1 << 1;

        inline bool support(Feat feat, Feat target) noexcept
        {
            return (feat & target) == target;
        }
    }

    struct Point
    {
        Point() = default;
        ~Point() = default;
        Point(const Point&) noexcept = default;
        Point(Point&&) noexcept = default;
        constexpr Point(int x, int y) : x(x), y(y) {}
        Point& operator=(const Point&) noexcept = default;
        Point& operator=(Point&&) noexcept = default;
        Point operator-() const noexcept { return { -x, -y }; }
        bool operator==(const Point& rhs) const noexcept { return x == rhs.x && y == rhs.y; }
        std::string to_string() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
        explicit operator std::string() const { return to_string(); }
        static constexpr Point right() { return { 1, 0 }; }
        static constexpr Point down() { return { 0, 1 }; }
        static constexpr Point left() { return { -1, 0 }; }
        static constexpr Point up() { return { 0, -1 }; }
        static constexpr Point zero() { return { 0, 0 }; }
        bool empty() const noexcept { return x == 0 && y == 0; }
        // for std::map
        bool operator<(const Point& rhs) const noexcept { return x < rhs.x || (x == rhs.x && y < rhs.y); }

        int x = 0;
        int y = 0;

        // clang-format off
#define DEFINE_ASST_POINT_BINARY_OP_AND_ARG_ASSIGN(Op)                    \
    friend Point operator Op(const Point& lhs, const Point& rhs) noexcept \
    {                                                                     \
        return { lhs.x Op rhs.x, lhs.y Op rhs.y };                        \
    }                                                                     \
    friend Point& operator Op##=(Point& val, const Point& opd) noexcept   \
    {                                                                     \
        val.x Op## = opd.x;                                               \
        val.y Op## = opd.y;                                               \
        return val;                                                       \
    }
        // clang-format on

        DEFINE_ASST_POINT_BINARY_OP_AND_ARG_ASSIGN(+)
            DEFINE_ASST_POINT_BINARY_OP_AND_ARG_ASSIGN(-)
            DEFINE_ASST_POINT_BINARY_OP_AND_ARG_ASSIGN(*)

#undef DEFINE_ASST_POINT_BINARY_OP_AND_ARG_ASSIGN

            friend Point operator*(int scalar, const Point& value) noexcept
        {
            return { value.x * scalar, value.y * scalar };
        }
        friend Point operator*(const Point& value, int scalar) noexcept
        {
            return { value.x * scalar, value.y * scalar };
        }
        static int dot(const Point& lhs, const Point& rhs) noexcept { return (lhs.x * rhs.x) + (lhs.y * rhs.y); }
        static double distance(const Point& lhs, const Point& rhs) noexcept
        {
            return std::sqrt(std::pow(rhs.x - lhs.x, 2) + std::pow(rhs.y - lhs.y, 2));
        }
        double length() const noexcept { return std::sqrt(static_cast<double>(dot(*this, *this))); }
    };

    struct Rect
    {
        Rect() = default;
        ~Rect() = default;
        Rect(const Rect&) noexcept = default;
        Rect(Rect&&) noexcept = default;
        Rect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
        Rect operator*(double rhs) const
        {
            return { x, y, static_cast<int>(width * rhs), static_cast<int>(height * rhs) };
        }
        int area() const noexcept { return width * height; }
        Rect center_zoom(double scale, int max_width = INT_MAX, int max_height = INT_MAX) const
        {
            const int half_width_scale = static_cast<int>(width * (1 - scale) / 2);
            const int half_height_scale = static_cast<int>(height * (1 - scale) / 2);
            Rect dst(x + half_width_scale, y + half_height_scale, static_cast<int>(width * scale),
                static_cast<int>(height * scale));
            if (dst.x < 0) {
                dst.x = 0;
            }
            if (dst.y < 0) {
                dst.y = 0;
            }
            if (dst.width + dst.x >= max_width) {
                dst.width = max_width - dst.x;
            }
            if (dst.height + dst.y >= max_height) {
                dst.height = max_height - dst.y;
            }
            return dst;
        }
        Rect& operator=(const Rect&) noexcept = default;
        Rect& operator=(Rect&&) noexcept = default;
        bool operator==(const Rect& rhs) const noexcept
        {
            return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
        }
        bool empty() const noexcept { return width == 0 || height == 0; }
        bool include(const Rect& rhs) const noexcept
        {
            return x <= rhs.x && y <= rhs.y && (x + width) >= (rhs.x + rhs.width) &&
                (y + height) >= (rhs.y + rhs.height);
        }
        bool include(const Point& rhs) const noexcept
        {
            return x <= rhs.x && y <= rhs.y && (x + width) >= rhs.x && (y + height) >= rhs.y;
        }
        std::string to_string() const
        {
            return "[ " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(width) + ", " +
                std::to_string(height) + " ]";
        }
        explicit operator std::string() const { return to_string(); }
        Rect move(Rect move) const { return { x + move.x, y + move.y, move.width, move.height }; }

        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
    };

    template <typename To, typename From>
    inline constexpr To make_rect(const From& rect)
    {
        return To{ rect.x, rect.y, rect.width, rect.height };
    }

    struct TextRect
    {
        std::string to_string() const
        {
            return "{ " + text + ": " + rect.to_string() + ", score: " + std::to_string(score) + " }";
        }
        explicit operator std::string() const { return to_string(); }

        Rect rect;
        double score = 0.0;
        std::string text;
    };

    struct MatchRect
    {
        std::string to_string() const
        {
            return "{ rect: " + rect.to_string() + ", score: " + std::to_string(score) + " }";
        }
        explicit operator std::string() const { return to_string(); }

        Rect rect;
        double score = 0.0;
    };
} // namespace asst

namespace std
{
    template <>
    struct hash<asst::Point>
    {
        size_t operator()(const asst::Point& point) const noexcept
        {
            return std::hash<int>()(point.x) ^ std::hash<int>()(point.y);
        }
    };

    template <>
    struct hash<asst::Rect>
    {
        size_t operator()(const asst::Rect& rect) const noexcept
        {
            return std::hash<int>()(rect.x) ^ std::hash<int>()(rect.y) ^ std::hash<int>()(rect.width) ^
                std::hash<int>()(rect.height);
        }
    };
}

namespace asst
{
    template <typename CType>
    struct ContainerHasher
    {
        std::size_t operator()(const CType& container) const
        {
            using value_type = typename CType::value_type;
            std::size_t seed = container.size();
            for (const value_type& e : container) {
                seed ^= std::hash<value_type>()(e) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    enum class AlgorithmType
    {
        Invalid = -1,
        JustReturn,
        MatchTemplate,
        OcrDetect,
        Hash
    };

    inline AlgorithmType get_algorithm_type(std::string algorithm_str)
    {
        utils::tolowers(algorithm_str);
        static const std::unordered_map<std::string_view, AlgorithmType> algorithm_map = {
            { "matchtemplate", AlgorithmType::MatchTemplate },
            { "justreturn", AlgorithmType::JustReturn },
            { "ocrdetect", AlgorithmType::OcrDetect },
            { "hash", AlgorithmType::Hash },
        };
        if (algorithm_map.contains(algorithm_str)) {
            return algorithm_map.at(algorithm_str);
        }
        return AlgorithmType::Invalid;
    }

    inline std::string enum_to_string(AlgorithmType algo)
    {
        static const std::unordered_map<AlgorithmType, std::string> algorithm_map = {
            { AlgorithmType::Invalid, "Invalid" },
            { AlgorithmType::JustReturn, "JustReturn" },
            { AlgorithmType::MatchTemplate, "MatchTemplate" },
            { AlgorithmType::OcrDetect, "OcrDetect" },
            { AlgorithmType::Hash, "Hash" },
        };
        if (auto it = algorithm_map.find(algo); it != algorithm_map.end()) {
            return it->second;
        }
        return "Invalid";
    }

    enum class ProcessTaskAction
    {
        Invalid = 0,
        BasicClick = 0x100,
        ClickSelf = BasicClick | 1, // �������λ��
        ClickRect = BasicClick | 2, // ���ָ������
        ClickRand = BasicClick | 4, // ����������
        DoNothing = 0x200,          // ʲô������
        Stop = 0x400,               // ֹͣ��ǰTask
        Swipe = 0x1000,             // ����
    };

    inline ProcessTaskAction get_action_type(std::string action_str)
    {
        utils::tolowers(action_str);
        static const std::unordered_map<std::string, ProcessTaskAction> action_map = {
            { "clickself", ProcessTaskAction::ClickSelf }, { "clickrand", ProcessTaskAction::ClickRand },
            { "", ProcessTaskAction::DoNothing },          { "donothing", ProcessTaskAction::DoNothing },
            { "stop", ProcessTaskAction::Stop },           { "clickrect", ProcessTaskAction::ClickRect },
            { "swipe", ProcessTaskAction::Swipe },
        };
        if (auto it = action_map.find(action_str); it != action_map.end()) {
            return it->second;
        }
        return ProcessTaskAction::Invalid;
    }

    inline std::string enum_to_string(ProcessTaskAction action)
    {
        static const std::unordered_map<ProcessTaskAction, std::string> action_map = {
            { ProcessTaskAction::Invalid, "Invalid" },     { ProcessTaskAction::BasicClick, "BasicClick" },
            { ProcessTaskAction::ClickSelf, "ClickSelf" }, { ProcessTaskAction::ClickRect, "ClickRect" },
            { ProcessTaskAction::ClickRand, "ClickRand" }, { ProcessTaskAction::DoNothing, "DoNothing" },
            { ProcessTaskAction::Stop, "Stop" },           { ProcessTaskAction::Swipe, "Swipe" },
        };
        if (auto it = action_map.find(action); it != action_map.end()) {
            return it->second;
        }
        return "Invalid";
    }
} // namespace asst

namespace asst
{
    // ������Ϣ
    struct TaskInfo
    {
        TaskInfo() = default;
        virtual ~TaskInfo() = default;
        TaskInfo(const TaskInfo&) = default;
        TaskInfo(TaskInfo&&) noexcept = default;
        TaskInfo& operator=(const TaskInfo&) = default;
        TaskInfo& operator=(TaskInfo&&) noexcept = default;
        std::string name;         // ������
        AlgorithmType algorithm = // ͼ���㷨����
            AlgorithmType::Invalid;
        ProcessTaskAction action = // Ҫ���еĲ���
            ProcessTaskAction::Invalid;
        std::vector<std::string> sub;           // �������б�
        bool sub_error_ignored = false;         // ���������ʧ���ˣ��Ƿ����ִ��ʣ�µ�����
        std::vector<std::string> next;          // ��һ�����ܵ������б�
        int max_times = INT_MAX;                // �������ִ�ж��ٴ�
        std::vector<std::string> exceeded_next; // �ﵽ��������֮����һ�����ܵ������б�
        std::vector<std::string> on_error_next; // �������֮��Ҫȥִ��ʲô
        std::vector<std::string> reduce_other_times; // ִ���˸��������Ҫ���ٱ�������ִ�д�����
        // ����ִ���˳�����ҩ����˵����һ�ε����ɫ��ʼ�ж���ťû��Ч��
        // ������ɫ��ʼ�ж�Ҫ-1
        Rect specific_rect;        // ָ������Ŀǰ�����ClickRect�������ã�����������
        int pre_delay = 0;         // ִ�и�����ǰ����ʱ
        int post_delay = 0;        // ִ�и���������ʱ
        int retry_times = INT_MAX; // δ�ҵ�ͼ��ʱ�����Դ���
        Rect roi;                  // Ҫʶ���������Ϊ0��ȫͼʶ��
        Rect rect_move;     // ʶ�����ƶ�����Щ���ʶ�𵽵ģ���Ҫ����Ĳ���ͬһ��λ�á�
        // ��ʶ����res�����res + result_move��λ��
        bool cache = false; // �Ƿ�ʹ�û�������
        std::vector<int> special_params; // ĳЩ������õ����������
    };

    // ����ʶ���������Ϣ
    struct OcrTaskInfo : public TaskInfo
    {
        OcrTaskInfo() = default;
        virtual ~OcrTaskInfo() override = default;
        OcrTaskInfo(const OcrTaskInfo&) = default;
        OcrTaskInfo(OcrTaskInfo&&) noexcept = default;
        OcrTaskInfo& operator=(const OcrTaskInfo&) = default;
        OcrTaskInfo& operator=(OcrTaskInfo&&) noexcept = default;
        std::vector<std::string> text; // ���ֵ�������ƥ�䵽��������һ��������ƥ������
        bool full_match = false;       // �Ƿ���Ҫȫƥ�䣬�����������Ӵ�����ƥ������
        bool is_ascii = false;         // �Ƿ������ַ�����ģ��
        bool without_det = false;      // �Ƿ�ʹ�ü��ģ��
        bool replace_full = false; // ƥ��֮���Ƿ������ַ���replace��false��ֻ�滻match�Ĳ��֣�
        std::vector<std::pair<std::string, std::string>>
            replace_map; // ������������ʶ����ַ���ǿ��replace֮���ٽ���ƥ��
    };

    // ͼƬƥ���������Ϣ
    struct MatchTaskInfo : public TaskInfo
    {
        MatchTaskInfo() = default;
        virtual ~MatchTaskInfo() override = default;
        MatchTaskInfo(const MatchTaskInfo&) = default;
        MatchTaskInfo(MatchTaskInfo&&) noexcept = default;
        MatchTaskInfo& operator=(const MatchTaskInfo&) = default;
        MatchTaskInfo& operator=(MatchTaskInfo&&) noexcept = default;
        std::vector<std::string> templ_names; // ƥ��ģ��ͼƬ�ļ���
        std::vector<double> templ_thresholds; // ģ��ƥ����ֵ
        std::pair<int, int> mask_range;       // ����Ķ�ֵ����Χ
    };

    // hash �����������Ϣ
    struct HashTaskInfo : public TaskInfo
    {
        HashTaskInfo() = default;
        virtual ~HashTaskInfo() override = default;
        HashTaskInfo(const HashTaskInfo&) = default;
        HashTaskInfo(HashTaskInfo&&) noexcept = default;
        HashTaskInfo& operator=(const HashTaskInfo&) = default;
        HashTaskInfo& operator=(HashTaskInfo&&) noexcept = default;
        std::vector<std::string> hashes; // ��Ҫ�����ϣֵ
        int dist_threshold = 0;          // ����������ֵ
        std::pair<int, int> mask_range;  // ����Ķ�ֵ����Χ
        bool bound = false;              // �Ƿ�ü���Χ�ڱ�
    };

    inline static const std::string UploadDataSource = "MeoAssistant";
} // namespace asst
