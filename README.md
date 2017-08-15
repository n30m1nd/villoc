
# Villoc

Villoc is a heap visualisation tool, it's a python script that renders a static
html file. An example can be seen here: http://wapiflapi.github.io/villoc/, this
is villoc running on an exploit of PlaidCTF 2015's challenge PlaidDB.

## How to

The easiest way to use villoc against a binary that uses "malloc" functions is probably to run the following command and open
out.html in a browser.

### Pintool way

```shell
ltrace ./target |& villoc.py - out.html;
```
It is probably a good idea to disable ASLR for repeatable results and to use a
file to pass the ltrace to villoc because otherwise the target's error output
will be interleaved and might confuse villoc sometimes.

```shell
setarch x86_64 -R ltrace -o trace ./target; villoc.py trace out.html;
```

### LD_PRELOAD way
**Use with caution, not very stable yet**
```shell
LD_PRELOAD="./ldpreload/libvilloc.so" ./target |& villoc.py - out.html
```

### frida-gadget way
```shell
LD_PRELOAD="/path/to/frida-gadget.so" FRIDA_GADGET_SCRIPT="./frida/malloc_hook.js" ./target |& villoc.py - out.html
```


![image](https://pbs.twimg.com/media/CDCQCzyWYAAck7k.png:large)


## Which malloc

This has been made with glibc's dl_malloc (and its derivates like ptmalloc2) in mind. But it should work for other
implementations, especially if you play with the `--header` and `--footer`
options to indicate how much overhead the targeted malloc adds to the user data.
