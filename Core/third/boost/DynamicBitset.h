#pragma once
#include <string>

namespace boost {

    template <typename Block, typename Allocator>
    class dynamic_bitset
    {
    public:
        typedef Block block_type;
        typedef Allocator allocator_type;
        typedef implementation - defined size_type;

        static const int bits_per_block = implementation - defined;
        static const size_type npos = implementation - defined;

        class reference
        {
            void operator&(); // not defined

        public:
            // An automatically generated copy constructor.

            reference& operator=(bool value);
            reference& operator=(const reference& rhs);

            reference& operator|=(bool value);
            reference& operator&=(bool value);
            reference& operator^=(bool value);
            reference& operator-=(bool value);

            bool operator~() const;
            operator bool() const;
            reference& flip();
        };

        typedef bool const_reference;

        explicit dynamic_bitset(const Allocator& alloc = Allocator());

        explicit dynamic_bitset(size_type num_bits, unsigned long value = 0,
            const Allocator& alloc = Allocator());

        template <typename CharT, typename Traits, typename Alloc>
        explicit dynamic_bitset(const std::basic_string<CharT, Traits, Alloc>& s,
            typename std::basic_string<CharT, Traits, Alloc>::size_type pos = 0,
            typename std::basic_string<CharT, Traits, Alloc>::size_type n = std::basic_string<CharT, Traits, Alloc>::npos,
            const Allocator& alloc = Allocator());

        template <typename BlockInputIterator>
        dynamic_bitset(BlockInputIterator first, BlockInputIterator last,
            const Allocator& alloc = Allocator());

        dynamic_bitset(const dynamic_bitset& b);

        void swap(dynamic_bitset& b);

        dynamic_bitset& operator=(const dynamic_bitset& b);

        allocator_type get_allocator() const;

        void resize(size_type num_bits, bool value = false);
        void clear();
        void push_back(bool bit);
        void append(Block block);

        template <typename BlockInputIterator>
        void append(BlockInputIterator first, BlockInputIterator last);

        dynamic_bitset& operator&=(const dynamic_bitset& b);
        dynamic_bitset& operator|=(const dynamic_bitset& b);
        dynamic_bitset& operator^=(const dynamic_bitset& b);
        dynamic_bitset& operator-=(const dynamic_bitset& b);
        dynamic_bitset& operator<<=(size_type n);
        dynamic_bitset& operator>>=(size_type n);
        dynamic_bitset operator<<(size_type n) const;
        dynamic_bitset operator>>(size_type n) const;

        dynamic_bitset& set(size_type n, bool val = true);
        dynamic_bitset& set();
        dynamic_bitset& reset(size_type n);
        dynamic_bitset& reset();
        dynamic_bitset& flip(size_type n);
        dynamic_bitset& flip();
        bool test(size_type n) const;
        bool any() const;
        bool none() const;
        dynamic_bitset operator~() const;
        size_type count() const;

        reference operator[](size_type pos);
        bool operator[](size_type pos) const;

        unsigned long to_ulong() const;

        size_type size() const;
        size_type num_blocks() const;
        size_type max_size() const;
        bool empty() const;

        bool is_subset_of(const dynamic_bitset& a) const;
        bool is_proper_subset_of(const dynamic_bitset& a) const;

        size_type find_first() const;
        size_type find_next(size_type pos) const;

    };


    template <typename B, typename A>
    bool operator==(const dynamic_bitset<B, A>& a, const dynamic_bitset<B, A>& b);

    template <typename Block, typename Allocator>
    bool operator!=(const dynamic_bitset<Block, Allocator>& a, const dynamic_bitset<Block, Allocator>& b);

    template <typename B, typename A>
    bool operator<(const dynamic_bitset<B, A>& a, const dynamic_bitset<B, A>& b);

    template <typename Block, typename Allocator>
    bool operator<=(const dynamic_bitset<Block, Allocator>& a, const dynamic_bitset<Block, Allocator>& b);

    template <typename Block, typename Allocator>
    bool operator>(const dynamic_bitset<Block, Allocator>& a, const dynamic_bitset<Block, Allocator>& b);

    template <typename Block, typename Allocator>
    bool operator>=(const dynamic_bitset<Block, Allocator>& a, const dynamic_bitset<Block, Allocator>& b);

    template <typename Block, typename Allocator>
    dynamic_bitset<Block, Allocator>
        operator&(const dynamic_bitset<Block, Allocator>& b1, const dynamic_bitset<Block, Allocator>& b2);

    template <typename Block, typename Allocator>
    dynamic_bitset<Block, Allocator>
        operator|(const dynamic_bitset<Block, Allocator>& b1, const dynamic_bitset<Block, Allocator>& b2);

    template <typename Block, typename Allocator>
    dynamic_bitset<Block, Allocator>
        operator^(const dynamic_bitset<Block, Allocator>& b1, const dynamic_bitset<Block, Allocator>& b2);

    template <typename Block, typename Allocator>
    dynamic_bitset<Block, Allocator>
        operator-(const dynamic_bitset<Block, Allocator>& b1, const dynamic_bitset<Block, Allocator>& b2);

    template <typename Block, typename Allocator, typename CharT, typename Alloc>
    void to_string(const dynamic_bitset<Block, Allocator>& b,
        std::basic_string<CharT, Alloc>& s);

    template <typename Block, typename Allocator, typename BlockOutputIterator>
    void to_block_range(const dynamic_bitset<Block, Allocator>& b,
        BlockOutputIterator result);

    template <typename CharT, typename Traits, typename Block, typename Allocator>
    std::basic_ostream<CharT, Traits>&
        operator<<(std::basic_ostream<CharT, Traits>& os, const dynamic_bitset<Block, Allocator>& b);

    template <typename CharT, typename Traits, typename Block, typename Allocator>
    std::basic_istream<CharT, Traits>&
        operator>>(std::basic_istream<CharT, Traits>& is, dynamic_bitset<Block, Allocator>& b);

} // namespace boost