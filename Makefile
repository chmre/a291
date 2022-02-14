.PHONY: clean All

All:
	@echo "----------Building project:[ a291unit - Debug ]----------"
	@cd "a291unit" && "$(MAKE)" -f  "a291unit.mk"
	@echo "----------Building project:[ a291_console - Debug ]----------"
	@cd "a291" && "$(MAKE)" -f  "a291_console.mk" && "$(MAKE)" -f  "a291_console.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ a291unit - Debug ]----------"
	@cd "a291unit" && "$(MAKE)" -f  "a291unit.mk"  clean
	@echo "----------Cleaning project:[ a291_console - Debug ]----------"
	@cd "a291" && "$(MAKE)" -f  "a291_console.mk" clean
