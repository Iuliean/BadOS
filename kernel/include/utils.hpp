#ifndef UTILS_HPP
#define UTILS_HPP

#define RETURN_IF_UNEXPECTED(v) do{\
    if(!v) return v;\
}while(0)

#define RETURN_IF_EXPECTED(v) do{\
    if(v) return v;\
}while(0)

#define PACKED __attribute__((packed))

namespace utils
{

}

#endif //UTILS_HPP
