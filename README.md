# stdr

Repeat stdin.

# BUILD

```
make
```

# USAGE

```
$ echo hoge | stdr 3
hoge
hoge
hoge
```

```
$ echo fuga | stdr
fuga
fuga
fuga
fuga
...
```

```
$ cat foo | stdr 3
1
1
2
2
(ctrl-d)
1
2
```

```
$ cat foo | stdr 2 -w
1
2
(ctrl-d)
1
2
1
2
```
