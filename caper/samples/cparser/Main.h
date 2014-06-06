#ifndef MAIN_H_
#define MAIN_H_

////////////////////////////////////////////////////////////////////////////
// CR_DeqSet<ITEM_T> -- deque and set

template <typename ITEM_T>
class CR_DeqSet : public std::deque<ITEM_T>
{
public:
    CR_DeqSet()
    {
    }

    CR_DeqSet(const CR_DeqSet<ITEM_T>& vs) : std::deque<ITEM_T>(vs)
    {
    }

    void operator=(const CR_DeqSet<ITEM_T>& vs)
    {
        this->assign(vs.begin(), vs.end());
    }

    virtual ~CR_DeqSet()
    {
    }

    void insert(const ITEM_T& item)
    {
        this->push_back(item);
    }

    bool Contains(const ITEM_T& item) const
    {
        const std::size_t siz = this->size();
        for (std::size_t i = 0; i < siz; i++)
        {
            if (this->at(i) == item)
                return true;
        }
        return false;
    }

    std::size_t Find(const ITEM_T& item) const
    {
        const std::size_t siz = this->size();
        for (std::size_t i = 0; i < siz; i++)
        {
            if (this->at(i) == item)
                return i;
        }
        return static_cast<std::size_t>(-1);
    }

    std::size_t Insert(const ITEM_T& item)
    {
        const std::size_t siz = this->size();
        for (std::size_t i = 0; i < siz; i++)
        {
            if (this->at(i) == item)
                return i;
        }
        this->push_back(item);
        return this->size() - 1;
    }

    void AddHead(const CR_DeqSet<ITEM_T>& items)
    {
        std::deque<ITEM_T>::insert(
            std::deque<ITEM_T>::begin(), items.begin(), items.end());
    }

    void AddTail(const CR_DeqSet<ITEM_T>& items)
    {
        std::deque<ITEM_T>::insert(
            std::deque<ITEM_T>::end(), items.begin(), items.end());
    }

    std::size_t count(const ITEM_T& item) const
    {
        std::size_t count = 0;
        for (std::size_t i : *this)
        {
            if (this->at(i) == item)
                count++;
        }
        return count;
    }

    void sort()
    {
        std::sort(this->begin(), this->end());
    }

    void unique()
    {
        std::unique(this->begin(), this->end());
    }

    void erase(const ITEM_T& item)
    {
        std::size_t i, j;
        const std::size_t count = this->size();
        for (i = j = 0; i < count; i++)
        {
            if (this->at(i) != item)
            {
                this->at(j++) = this->at(i);
            }
        }
        if (i != j)
            this->resize(j);
    }
};

namespace std
{
    template <typename ITEM_T>
    inline void swap(CR_DeqSet<ITEM_T>& vs1, CR_DeqSet<ITEM_T>& vs2)
    {
        vs1.swap(vs2);
    }
}

////////////////////////////////////////////////////////////////////////////
// CR_String

typedef std::string CR_String;

////////////////////////////////////////////////////////////////////////////
// CR_StringSet

typedef CR_DeqSet<CR_String> CR_StringSet;

////////////////////////////////////////////////////////////////////////////
// CR_Map<from, to>, CR_UnorderedMap<from, to>

#define CR_Map              std::map
#define CR_UnorderedMap     std::unordered_map

////////////////////////////////////////////////////////////////////////////

#endif  // ndef MAIN_H_
