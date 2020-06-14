#include "../Singleton.hpp"

class DeriveSingle : public Singleton<DeriveSingle>
{
public:
    DeriveSingle(token)
    {
        std::cout<<"destructor called!"<<std::endl;
    }
    ~DeriveSingle()
    {
        std::cout<<"constructor called!"<<std::endl;
    }
    DeriveSingle(const DeriveSingle&) = delete;
    DeriveSingle operator = (const DeriveSingle&) = delete;
};

int main(int argc, char* argv[])
{
    DeriveSingle& instance1 = DeriveSingle::get_instance();
    DeriveSingle& instance2 = DeriveSingle::get_instance();
    return 0;
}