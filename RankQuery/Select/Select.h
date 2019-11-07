#include"../Rank/Rank.h"


class select_support {
public:
	rank_support* r1;
	select_support(rank_support* R);
	
    uint64_t select1(uint64_t i) ;

	uint64_t select0(uint64_t i) ;
	uint64_t overhead() ;
};