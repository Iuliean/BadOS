#ifndef UTILS_HPP
#define UTILS_HPP

#define RETURN_IF_UNEXPECTED(v) do{\
    if(!v) return v;\
}while(0)

#define RETURN_IF_EXPECTED(v) do{\
    if(v) return v;\
}while(0)

namespace utils
{

}

#endif //UTILS_HPP
