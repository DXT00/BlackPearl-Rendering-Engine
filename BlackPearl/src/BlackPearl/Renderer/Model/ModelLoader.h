namespace BlackPearl
{
    class ModelLoader
    {
    private:
        /* data */
    public:
        ModelLoader(/* args */);
        ~ModelLoader();
        virtual void Load(const std::string& path) = 0;
    };
    

    
} // namespace BlackPearl
