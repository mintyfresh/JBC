
# include <stdlib.h>
# include <string.h>

# include "Debug.h"
# include "Defines.h"
# include "ClassFile.h"
# include "AttributeInfo.h"

void deleteCodeAttribute(CodeAttribute *code) {
	if(code->code != NULL)
		DELETE(code->code);
	if(code->exception_table != NULL) {
		debug_printf(level3, "Deleting exception table.\n");
		deleteList(code->exception_table, DELETE);
	}
	if(code->attributes != NULL) {
		debug_printf(level3, "Deleting code attributes.\n");
		deleteList(code->attributes, deleteAttribute);
	}
	DELETE(code);
}

void deleteStackMapOffFrame(StackMapOffFrame *frame) {
	DELETE(frame);
}

void deleteStackMapItemFrame(StackMapItemFrame *frame) {
	if(frame->stack != NULL)
		DELETE(frame->stack);
	DELETE(frame);
}

void deleteStackMapExtFrame(StackMapExtFrame *frame) {
	if(frame->stack != NULL)
		DELETE(frame->stack);
	DELETE(frame);
}

void deleteStackMapListFrame(StackMapListFrame *frame) {
	int idx, count = frame->tag - 251;
	if(frame->stack != NULL) {
		for(idx = 0; idx < count; idx++)
			DELETE(frame->stack[idx]);
		DELETE(frame->stack);
	}
	DELETE(frame);
}

void deleteStackMapFullFrame(StackMapFullFrame *frame) {
	if(frame->locals != NULL)
		deleteList(frame->locals, DELETE);
	if(frame->stack != NULL)
		deleteList(frame->stack, DELETE);
	DELETE(frame);
}

void deleteStackMapFrame(StackMapFrame *frame) {
	if(frame->tag <= 63) {
		DELETE(frame);
	} else
	// Stack Map Same Locals 1
	if(frame->tag <= 127) {
		deleteStackMapItemFrame((void *)frame);
	} else
	// Reserved Values
	if(frame->tag <= 246) {
		DELETE(frame);
	} else
	// Stack Map Same Locals 1 Extended
	if(frame->tag == 247) {
		deleteStackMapExtFrame((void *)frame);
	} else
	// Stack Map Chop Frame
	if(frame->tag <= 250) {
		deleteStackMapOffFrame((void *)frame);
	} else
	// Stack Map Same Frame Extended
	if(frame->tag == 251) {
		deleteStackMapOffFrame((void *)frame);
	} else
	// Stack Map Append Frame
	if(frame->tag <= 254) {
		deleteStackMapListFrame((void *)frame);
	}
	// Stack Map Full Frame
	else {
		deleteStackMapFullFrame((void *)frame);
	}
}

void deleteStackMapTableAttribute(StackMapTableAttribute *table) {
	if(table->entries != NULL)
		deleteList(table->entries, deleteStackMapFrame);
	DELETE(table);
}

void deleteExceptionsAttribute(ExceptionsAttribute *except) {
	if(except->exception_table != NULL)
		deleteList(except->exception_table, NULL);
	DELETE(except);
}

void deleteInnerClassesAttribute(InnerClassesAttribute *inner) {
	if(inner->classes != NULL)
		deleteList(inner->classes, DELETE);
	DELETE(inner);
}

void deleteLineNumberTableAttribute(LineNumberTableAttribute *table) {
	if(table->line_number_table != NULL)
		deleteList(table->line_number_table, DELETE);
	DELETE(table);
}

void deleteLocalVariableTableAttribute(LocalVariableTableAttribute *table) {
	if(table->local_variable_table != NULL)
		deleteList(table->local_variable_table, DELETE);
	DELETE(table);
}

void deleteLocalVariableTypeTableAttribute(LocalVariableTypeTableAttribute *table) {
	if(table->local_variable_type_table != NULL)
		deleteList(table->local_variable_type_table, DELETE);
	DELETE(table);
}

void deleteElementValue(ElementValue *value) {
	extern void deleteAnnotationEntry(AnnotationEntry *);

	switch(value->tag) {
		case '@':
			deleteAnnotationEntry(value->value.annotation_value);
			break;
		case '[':
			if(value->value.array_values != NULL)
				deleteList(value->value.array_values, deleteElementValue);
			break;
	}
	DELETE(value);
}

void deleteElementValuePairsEntry(ElementValuePairsEntry *entry) {
	if(entry->value != NULL)
		deleteElementValue(entry->value);
	DELETE(entry);
}

void deleteAnnotationEntry(AnnotationEntry *entry) {
	if(entry->element_value_pairs != NULL)
		deleteList(entry->element_value_pairs, deleteElementValuePairsEntry);
	DELETE(entry);
}

void deleteRuntimeAnnotationsAttribute(RuntimeAnnotationsAttribute *annot) {
	if(annot->annotations != NULL)
		deleteList(annot->annotations, deleteAnnotationEntry);
	DELETE(annot);
}

void deleteParameterAnnotationsEntry(ParameterAnnotationsEntry *entry) {
	if(entry->annotations != NULL)
		deleteList(entry->annotations, deleteAnnotationEntry);
	DELETE(entry);
}

void deleteRuntimeParameterAnnotationsAttribute(RuntimeParameterAnnotationsAttribute *annot) {
	if(annot->parameter_annotations != NULL)
		deleteList(annot->parameter_annotations, deleteParameterAnnotationsEntry);
	DELETE(annot);
}

void deleteAnnotationDefaultAttribute(AnnotationDefaultAttribute *value) {
	if(value->default_value != NULL)
		deleteElementValue(value->default_value);
	DELETE(value);
}

void deleteBootstrapMethodEntry(BootstrapMethodEntry *entry) {
	if(entry->bootstrap_arguments != NULL)
		deleteList(entry->bootstrap_arguments, NULL);
	DELETE(entry);
}

void deleteBootstrapMethodsAttribute(BootstrapMethodsAttribute *bootstrap) {
	if(bootstrap->bootstrap_methods != NULL)
		deleteList(bootstrap->bootstrap_methods, deleteBootstrapMethodEntry);
	DELETE(bootstrap);
}

void deleteAttribute(AttributeInfo *info) {
	ConstantUtf8Info *name;
	if(info == NULL) return;

	name = info->name;
	if(name == NULL || name->bytes == NULL) {
		DELETE(info);
		return;
	}

	debug_printf(level2, "Deleting attribute : %s.\n", name->bytes);

	// Code Attribute
	if(!strcmp("Code", (char *)name->bytes)) {
		deleteCodeAttribute((void *)info);
	} else
	// Stack Map Table Attribute
	if(!strcmp("StackMapTable", (char *)name->bytes)) {
		deleteStackMapTableAttribute((void *)info);
	} else
	// Exceptions Attribute
	if(!strcmp("Exceptions", (char *)name->bytes)) {
		deleteExceptionsAttribute((void *)info);
	} else
	// Inner Classes Attribute
	if(!strcmp("InnerClasses", (char *)name->bytes)) {
		deleteInnerClassesAttribute((void *)info);
	} else
	// Source Debug Extension Attribute
	if(!strcmp("SourceDebugExtension", (char *)name->bytes)) {
		DELETE(((SourceDebugExtensionAttribute *)info)->debug_extension);
		DELETE(info);
	} else
	// Line Number Table Attribute
	if(!strcmp("LineNumberTable", (char *)name->bytes)) {
		deleteLineNumberTableAttribute((void *)info);
	} else
	// Local Variable Table Attribute
	if(!strcmp("LocalVariableTable", (char *)name->bytes)) {
		deleteLocalVariableTableAttribute((void *)info);
	} else
	// Local Variable Type Table Attribute
	if(!strcmp("LocalVariableTypeTable", (char *)name->bytes)) {
		deleteLocalVariableTypeTableAttribute((void *)info);
	} else
	// Runtime Visible Annotations Attribute
	if(!strcmp("RuntimeVisibleAnnotations", (char *)name->bytes)) {
		deleteRuntimeAnnotationsAttribute((void *)info);
	} else
	// Runtime Invisible Annotations Attribute
	if(!strcmp("RuntimeInvisibleAnnotations", (char *)name->bytes)) {
		deleteRuntimeAnnotationsAttribute((void *)info);
	} else
	// Runtime Visible Parameter Annotations Attribute
	if(!strcmp("RuntimeVisibleParameterAnnotations", (char *)name->bytes)) {
		deleteRuntimeParameterAnnotationsAttribute((void *)info);
	} else
	// Runtime Visible Parameter Annotations Attribute
	if(!strcmp("RuntimeInvisibleParameterAnnotations", (char *)name->bytes)) {
		deleteRuntimeParameterAnnotationsAttribute((void *)info);
	} else
	// Annotation Default Attribute
	if(!strcmp("AnnotationDefault", (char *)name->bytes)) {
		deleteAnnotationDefaultAttribute((void *)info);
	} else
	// Bootstrap Methods Attribute
	if(!strcmp("BootstrapMethods", (char *)name->bytes)) {
		deleteBootstrapMethodsAttribute((void *)info);
	} else {
		DELETE(info);
	}
}
