#invoke make inside following directories and in this order: loader, launch, fib
#move the lib_simpleloader.so and launch binaries inside bin directory
#Provide the command for cleanup
# .PHONY: all loader launch fib clean
name1=launch
name2=fib
dir1=launcher
dir2=bin
dir3=test
dir4=loader
all: work1 work2 work3 work4 work5

work1:
	make -C $(dir4)

work2:
	make -C $(dir1)

work3:
	make -C $(dir3)
#cp $(src)/$(file_copy) $(drc)/$(file_copy)
work4:
	mv $(dir1)/$(name1) $(dir2)/
#mv test/fib bin/
work5:
	cd $(dir2) && ./$(name1) ../$(dir3)/$(name2)

clean:
	make -C $(dir4) clean
	make -C $(dir1) clean
	make -C $(dir3) clean
	rm -r $(dir2)/*

	
	