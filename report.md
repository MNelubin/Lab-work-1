# Lab Work 1 Report: Parallel Image Processing

## Task Description

Update the solution for Lab Work 1 (image processing from the first semester) by adding parallelism to the implementation. Note that rewriting the code from scratch is not considered a solution.

Conduct a reproducible experiment that justifies that the new solution works faster than the old one, and write a report on it.

## Hardware Specifications

The experiment was conducted on the following hardware:

- **Storage:** 954 GB (674 GB used)
- **Video Adapter:** 128 MB Intel(R) Iris(R) Xe Graphics
- **RAM:** 32.0 GB, Speed: 6400 MHz
- **Processor:** 13th Gen Intel(R) Core(TM) i9-13900H @ 2.60 GHz

## Images Used for Experiment

The tests were performed on the following images:

- <mcfile path="examples_BMP/bmp_24.bmp" name="examples_BMP/bmp_24.bmp"></mcfile>
- <mcfile path="examples_BMP/sample1.bmp" name="examples_BMP/sample1.bmp"></mcfile>

## Experiment Results

The following results were obtained from running the program:

```
max@MaxLaptop:/mnt/d/education/C++/first semestr/Lab-work-1-main/Lab-work-1-main$ ./lgo 
What file path? 
examples_BMP/sample1.bmp 
Do you need rotated images? (1/0) 
1 
Do you need rotated image with Gaus filtration? (1/0) 
1 
Sequential methods time: 0.553313 seconds 
Parallel methods time: 0.0758145 seconds
```

## Analysis

The experiment clearly shows a significant performance improvement when using parallel processing methods compared to sequential methods. The parallel execution time (0.0758145 seconds) is substantially lower than the sequential execution time (0.553313 seconds) for the given image processing tasks (rotation and Gaussian filtering) on the `sample1.bmp` image.

This speedup is achieved by leveraging multiple processor cores to perform computations concurrently, as implemented using OpenMP in the C++ code. The parallel versions of `rotate90Clockwise`, `rotate90ContClockwise`, and `applyGaussianFilter` distribute the workload across available threads, leading to a faster overall execution time.

## Conclusion

The addition of parallelism to the image processing solution has successfully resulted in a more performant application. The experimental results validate that the parallel implementation is significantly faster than the original sequential version, fulfilling the requirements of the task.