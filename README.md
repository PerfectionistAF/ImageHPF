# ImageHPF
mpiexec -n 2 .\mpi.exe .\image.jpg .\mpi_m.jpg 11 7 (kernel size and sigma)
.\serial.exe .\image.jpg .\new_s.jpg 11 7
.\openmp.exe .\image.jpg .\new_p.jpg 11 7

![hpf](https://github.com/PerfectionistAF/ImageHPF/assets/77901496/4440140c-6e4b-4d37-aad3-28abcf2d1980)


![sequential](https://github.com/PerfectionistAF/ImageHPF/assets/77901496/fcdb013b-fb5d-4ecc-a43b-5dce75997ccd)







