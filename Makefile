all:
	make -C grbl $@
	make -C board/st/stm32f4/mb997 $@

clean:
	make -C grbl $@
	make -C board/st/stm32f4/mb997 $@
