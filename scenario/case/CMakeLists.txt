FILE(GLOB sub_dirs ./*)
FOREACH (d ${sub_dirs})
    IF (IS_DIRECTORY ${curdir}/${d})
        ADD_SUBDIRECTORY(${d})
    ENDIF ()
ENDFOREACH ()