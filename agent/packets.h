#ifndef PACKETS_H
# define PACKETS_H
//위에 이름 바꾸셈

//#include "object.h"
//#include "../utils/utils.h"
/*
 * 어찌 짤것임?
 * -> 구조체를 만들어야지
 *  header, body를 어찌 만들것인가
 */

/*
typedef enum e_type
{
	cpu,
	mem,
	pack,
	proc
} types;
*/

typedef struct s_packet
{
	//대충 헤더
	char type_n_date[42];
	int size; 
//	void *body;
	//대충 바디
} packet;


#endif
