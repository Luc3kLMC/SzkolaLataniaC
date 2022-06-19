cmake_minimum_required(VERSION 2.8.5)
project(szkola LANGUAGES C)

# ACE
add_subdirectory(ACE ace)

add_executable(
	szkola.elf main.c menu.c 
	
) # Compiler emits .elf file, we need hunk for Amiga

target_compile_options(szkola.elf PRIVATE -Wall -Wextra)

target_link_libraries(szkola.elf ace)
target_compile_definitions(ace PUBLIC ACE_DEBUG ACE_DEBUG_UAE)

add_custom_command( # So call elf2hunk
	TARGET szkola.elf POST_BUILD
	COMMAND ${ELF2HUNK} szkola.elf szkola.exe
)

set(RES_DIR ${CMAKE_CURRENT_LIST_DIR}/res) # st�d bierzemy pliki do konwersji
set(DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/data) # tu maj� l�dowa�
file(MAKE_DIRECTORY ${DATA_DIR}) #za���my data





set(PLT_PATH ${DATA_DIR}/szkola.plt) #tak si� b�dzie nazywa� docelowa paleta
convertPalette(szkola.elf ${RES_DIR}/szkola.pal ${PLT_PATH}) # z res/amimage.pal do build/data/amimage.plt

convertBitmaps(
	TARGET szkola.elf PALETTE ${PLT_PATH}
	INTERLEAVED SOURCES
		${RES_DIR}/card2.png ${RES_DIR}/card3.png ${RES_DIR}/card4.png ${RES_DIR}/card5.png ${RES_DIR}/card6.png
		${RES_DIR}/card7.png ${RES_DIR}/card8.png ${RES_DIR}/card9.png ${RES_DIR}/card10.png
		${RES_DIR}/card11.png ${RES_DIR}/card12.png ${RES_DIR}/card13.png  
	DESTINATIONS
		${DATA_DIR}/card2.bm ${DATA_DIR}/card3.bm ${DATA_DIR}/card4.bm ${DATA_DIR}/card5.bm ${DATA_DIR}/card6.bm
		${DATA_DIR}/card7.bm ${DATA_DIR}/card8.bm ${DATA_DIR}/card9.bm ${DATA_DIR}/card10.bm
		${DATA_DIR}/card11.bm ${DATA_DIR}/card12.bm ${DATA_DIR}/card13.bm
)



file(
	COPY ${RES_DIR}/placeholdermod.mod ${RES_DIR}/topaz.fnt
	DESTINATION ${DATA_DIR}
)
