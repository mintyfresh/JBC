# ifndef __CONSTANTINFO_H__
# define __CONSTANTINFO_H__

# include <stdint.h>

# include "Defines.h"

# include "Types.h"
# include "ClassBuffer.h"
# include "ClassBuilder.h"

ConstantInfo *createConstant(uint8_t type);

void deleteConstant(ConstantInfo *info);

int encodeConstant(ClassBuilder *builder, ConstantInfo *info);

ConstantInfo *decodeConstant(ClassBuffer *buffer);

static inline
int isLongConstant(ConstantInfo *info) {
	if(info == NULL) return 0;
	return info->tag == CONSTANT_LONG ||
		info->tag == CONSTANT_DOUBLE;
}

# endif /* ConstantInfo.h */
