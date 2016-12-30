SET(coverage_info "${CMAKE_BINARY_DIR}/${_outputname}.info")
SET(coverage_cleaned "${coverage_info}.cleaned")
SET(coverage_report "${coverage_info}.report")
SET(coverage_badges "${CMAKE_CURRENT_SOURCE_DIR}/.badges")

ADD_CUSTOM_COMMAND(
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/cmake/create_badges.sh ${coverage_report} ${coverage_badges}
)

