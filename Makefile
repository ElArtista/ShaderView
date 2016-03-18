#=- Makefile -=#
# CreateProcess NULL bug
SHELL = cmd.exe
#
# Project settings
#
TARGETNAME = $(notdir $(CURDIR))
LIBS = glu32 opengl32 gdi32

#
# Compiler settings
#
# Compiler
CC = gcc
# Compiler flags
#CFLAGS = -O2 -Wall -Wextra
CFLAGS = -g -Wall -Wextra
# Linker
LD = gcc
# Linker flags
LDFLAGS = -static -static-libgcc

#
# Project directory settings
#
# Source directory
SRCDIR = src
# Build directory
BUILDDIR = tmp
# Target directory
TARGETDIR = bin
# Dependency directory
DEPSDIR = deps

#
# Helpers
#
# Recursive wildcard func
rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard, $d/, $2) $(filter $(subst *, %, $2), $d))

# Suppress command errors
ifeq ($(OS), Windows_NT)
	suppress_out = > nul 2>&1 || (exit 0)
else
	suppress_out => /dev/null 2>&1 || true
endif

# Native paths
ifeq ($(OS), Windows_NT)
	native_path = $(subst /,\,$(1))
else
	native_path = $(1)
endif

# Makedir command
MKDIR_CMD = mkdir
mkdir = -$(if $(wildcard $(1)/.*), , $(MKDIR_CMD) $(call native_path, $(1)) $(suppress_out))

# Rmdir command
RMDIR_CMD = rmdir /s /q
rmdir = $(if $(wildcard $(1)/.*), $(RMDIR_CMD) $(call native_path, $(1)),)

# Os extension
ifeq ($(OS), Windows_NT)
	OSEXT = .exe
endif

#
# Generated values
#
# Sources
SRC = $(call rwildcard, $(SRCDIR), *.c)
# Objects
OBJ = $(foreach obj, $(SRC:.c=.o), $(BUILDDIR)/$(obj))
# Output
OUTPUT = $(TARGETNAME)$(OSEXT)
# Dependencies
DEPS = $(strip $(sort $(dir $(wildcard $(DEPSDIR)/*/))))
# Include directories
INCDIR = $(strip $(foreach dep, $(DEPS), -I$(dep)include))
# Library directories
LIBDIR = $(strip $(foreach dep, $(DEPS), -L$(dep)lib))
# Library flags
LIBFLAGS = $(strip $(foreach lib, $(LIBS), -l$(lib)))

# Dep objects
SRC += $(foreach dep, $(DEPS), $(call rwildcard, $(dep), *.c))
OBJ += $(foreach obj, $(SRC:.c=.o), $(BUILDDIR)/$(obj))

#
# Rules
#
# Disable builtin rules
.SUFFIXES:

# Main target
$(TARGETDIR)/$(OUTPUT): $(OBJ)
	@echo [+] Linking $@
	@$(call mkdir, $(@D))
	@$(LD) $(LDFLAGS) $(LIBDIR) -o $@ $? $(LIBFLAGS)

# Source compile
$(BUILDDIR)/%.o: %.c
	@echo [^>] Compiling $<
	@$(call mkdir, $(@D))
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

clean:
	@echo Cleaning...
	@$(call rmdir, $(BUILDDIR))

# Non file targets
.PHONY: deps $(DEPS) clean
