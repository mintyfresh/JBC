# ifndef __CLASSBUILDER_H__
# define __CLASSBUILDER_H__

# include <stdio.h>
# include <stdint.h>

struct BuilderError {
	const char *msg;

	inline
	BuilderError(const char *msg)
		: msg(msg) {
	}
};

class ClassBuilder {
private:
	FILE *output;

public:
	ClassBuilder(FILE *output);
	~ClassBuilder();

public:
	size_t Position();

	ClassBuilder *NextByte(uint8_t byte);

	ClassBuilder *NextShort(uint16_t word);

	ClassBuilder *NextInt(uint32_t dword);
};

// ClassBuilder *createBuilder(FILE *classFile);
# define createBuilder(file) new ClassBuilder(file)

// void deleteBuilder(ClassBuilder *builder);
# define deleteBuilder(builder) delete builder

// long int builderPos(ClassBuilder *builder);
# define builderPos(builder) builder->Position()

// ClassBuilder *buildNextByte(ClassBuilder *builder, uint8_t byte);
# define buildNextByte(builder, byte) builder->NextByte(byte)

// ClassBuilder *buildNextShort(ClassBuilder *builder, uint16_t word);
# define buildNextShort(builder, word) builder->NextShort(word)

// ClassBuilder *buildNextInt(ClassBuilder *builder, uint32_t dword);
# define buildNextInt(builder, dword) builder->NextInt(dword)

# endif /* ClassBuilder.h */
