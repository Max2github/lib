# Welcome to my own libraries
First of all you should know that there may be some bugs.
I tried to only commit those, which have everything working,
but you know...

## Using them directly
### Header files
The header files are in this folder, in the directory "freestanding"
and in the folder "templates".

They often have the ending "_h.h".
Header files in the "template" folder are header-only libraries.

### Source files
All source files are in the directory "src".
They are also named .h, please do not ask me why.<br>
What I did (for msh -> see mshgit):<br>
I made a all.c file, in which I included all sourcefiles.

## Building
There is a little Makefile.<br>
Can can build the libraries (.a files -> linux style)
for words and list:
- `make words`
- `make list`

## Installing
You can install the libraries:
- def.h : `make target=def install`
- simple_list.h : `make target=simple_list install`
- simple_array.h : `make target=simple_array install`

- words : build it + `make target=words install`
- list : build it + `make target=list install`