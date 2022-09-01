find_package(Python3 COMPONENTS Interpreter Development)

set(SVIOPGen ${CMAKE_CURRENT_LIST_DIR}/main.py)

function(SVIOPGenerate TSLExtensionsList)
    execute_process(
            COMMAND python3 ${SVIOPGen} --extensions ${TSLExtensionsList}
#            OUTPUT_VARIABLE ExtensionsList
#            RESULT_VARIABLE ReturnValue
    )
endfunction()