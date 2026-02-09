## Mandelbrot example for node_crunch2.

To build (in the top folder):

```bash
xmake build mandel_nc
```

To start the server:

```bash
mandel_nc --server &
```

To start some nodes:

```bash
mandel_nc &
mandel_nc &
mandel_nc &
mandel_nc &
```

Or you can use the bash script:

```bash
run_mandel.sh
```
