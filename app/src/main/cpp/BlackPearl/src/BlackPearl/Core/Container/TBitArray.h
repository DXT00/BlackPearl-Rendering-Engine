#pragma once
namespace BlackPearl {
	class TBitArray
	{
    public:
        // ���캯������ʼ��λ�����С��Ĭ�ϴ�С 32 λ
        // 1024 bit(32 x32)
        TBitArray(size_t size = 32) {
            resize(size);
        }

        // ����λ��index ��λ��λ�ã�0 �� 1��
        void setBit(size_t index, bool value) {
            if (index >= size) {
                throw std::out_of_range("Index out of range");
            }

            size_t arrayIndex = index / 32;
            size_t bitIndex = index % 32;

            if (value) {
                bits[arrayIndex] |= (1 << bitIndex);  // ����Ϊ 1
            }
            else {
                bits[arrayIndex] &= ~(1 << bitIndex); // ����Ϊ 0
            }
        }

        // ��ȡλֵ
        bool getBit(size_t index) const {
            if (index >= size) {
                throw std::out_of_range("Index out of range");
            }

            size_t arrayIndex = index / 32;
            size_t bitIndex = index % 32;

            return (bits[arrayIndex] >> bitIndex) & 1;
        }

        // ���� [] ����������ȡ������ָ����������λ
        bool operator[](size_t index) const {
            if (index >= size) {
                throw std::out_of_range("Index out of range");
            }

            size_t arrayIndex = index / 32;
            size_t bitIndex = index % 32;

            return (bits[arrayIndex] & (1 << bitIndex)) != 0;
        }

        // ���� [] ������������ָ����������λ
        bool& operator[](size_t index) {
            if (index >= size) {
                throw std::out_of_range("Index out of range");
            }

            size_t arrayIndex = index / 32;
            size_t bitIndex = index % 32;

            // ͨ�����÷���λ�������޸�
            bool& bitRef = *reinterpret_cast<bool*>(&bits[arrayIndex]);
            return bitRef;
        }
        // ��չλ�����С
        void resize(size_t newSize) {
            if (newSize <= size) return;

            size_t newArraySize = (newSize + 31) / 32; // ����ȡ��
            bits.resize(newArraySize, 0);  // ʹ���µĴ�С����ʼ��Ϊ 0
            size = newSize;
        }

        // ��תλ�����е�����λ
        void invert() {
            for (auto& block : bits) {
                block = ~block;
            }
        }

        // ���λ���飨��������λΪ 0��
        void clear() {
            std::fill(bits.begin(), bits.end(), 0);
        }

        // ��ӡλ���飨�����ã�
        void print() const {
            for (size_t i = 0; i < size; ++i) {
                std::cout << getBit(i);
                if (i % 8 == 7) std::cout << " ";  // ÿ 8 λ��ӡһ���ո�
            }
            std::cout << std::endl;
        }

    private:
        size_t size;                  // λ������ܴ�С����λ��λ��
        std::vector<uint32_t> bits;   // ���ڴ洢λ���ݣ�ÿ�� uint32_t �洢 32 λ
	};


}
