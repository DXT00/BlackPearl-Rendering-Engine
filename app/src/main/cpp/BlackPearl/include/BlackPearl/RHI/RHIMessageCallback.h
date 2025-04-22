#pragma once
namespace BlackPearl {


    enum class MessageSeverity : uint8_t
    {
        Info,
        Warning,
        Error,
        Fatal
    };
    class IMessageCallback
    {
    protected:
        IMessageCallback() = default;
        virtual ~IMessageCallback() = default;

    public:
        // NVRHI will call message(...) whenever it needs to signal something.
        // The application is free to ignore the messages, show message boxes, or terminate.
        virtual void message(MessageSeverity severity, const char* messageText) = 0;

        IMessageCallback(const IMessageCallback&) = delete;
        IMessageCallback(const IMessageCallback&&) = delete;
        IMessageCallback& operator=(const IMessageCallback&) = delete;
        IMessageCallback& operator=(const IMessageCallback&&) = delete;
    };


    struct DefaultMessageCallback : public IMessageCallback
    {
        static DefaultMessageCallback& GetInstance()
        {
            static DefaultMessageCallback s_instance;
            return s_instance;
        }

        void message(MessageSeverity severity, const char* messageText) override
        {
            std::cout << messageText << std::endl;
        }
    };
}

