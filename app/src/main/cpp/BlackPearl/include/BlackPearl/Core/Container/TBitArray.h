#pragma once
namespace BlackPearl {
	class TBitArray
	{
    public:
        // 构造函数，初始化位数组大小，默认大小 32 位
        // 1024 bit(32 x32)
        TBitArray(size_t size = 32) {
            resize(size);
        }

        // 设置位，index 是位的位置（0 或 1）
        void setBit(size_t index, bool value) {
            if (index >= size) {
                throw std::out_of_range("Index out of range");
            }

            size_t arrayIndex = index / 32;
            size_t bitIndex = index % 32;

            if (value) {
                bits[arrayIndex] |= (1 << bitIndex);  // 设置为 1
            }
            else {
                bits[arrayIndex] &= ~(1 << bitIndex); // 设置为 0
            }
        }

        // 获取位值
        bool getBit(size_t index) const {
            if (index >= size) {
                throw std::out_of_range("Index out of range");
            }

            size_t arrayIndex = index / 32;
            size_t bitIndex = index % 32;

            return (bits[arrayIndex] >> bitIndex) & 1;
        }

        // 重载 [] 操作符来获取或设置指定索引处的位
        bool operator[](size_t index) const {
            if (index >= size) {
                throw std::out_of_range("Index out of range");
            }

            size_t arrayIndex = index / 32;
            size_t bitIndex = index % 32;

            return (bits[arrayIndex] & (1 << bitIndex)) != 0;
        }

        // 重载 [] 操作符来设置指定索引处的位
        bool& operator[](size_t index) {
            if (index >= size) {
                throw std::out_of_range("Index out of range");
            }

            size_t arrayIndex = index / 32;
            size_t bitIndex = index % 32;

            // 通过引用返回位，允许修改
            bool& bitRef = *reinterpret_cast<bool*>(&bits[arrayIndex]);
            return bitRef;
        }
        // 扩展位数组大小
        void resize(size_t newSize) {
            if (newSize <= size) return;

            size_t newArraySize = (newSize + 31) / 32; // 向上取整
            bits.resize(newArraySize, 0);  // 使用新的大小并初始化为 0
            size = newSize;
        }

        // 反转位数组中的所有位
        void invert() {
            for (auto& block : bits) {
                block = ~block;
            }
        }

        // 清空位数组（设置所有位为 0）
        void clear() {
            std::fill(bits.begin(), bits.end(), 0);
        }

        // 打印位数组（调试用）
        void print() const {
            for (size_t i = 0; i < size; ++i) {
                std::cout << getBit(i);
                if (i % 8 == 7) std::cout << " ";  // 每 8 位打印一个空格
            }
            std::cout << std::endl;
        }

    private:
        size_t size;                  // 位数组的总大小（单位：位）
        std::vector<uint32_t> bits;   // 用于存储位数据，每个 uint32_t 存储 32 位
	};


}
