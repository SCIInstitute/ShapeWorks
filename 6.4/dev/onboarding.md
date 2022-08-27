# ShapeWorks Developer Onboarding

Welcome to ShapeWorks Development!

This page contains pointers to help get you started building and developing with ShapeWorks.

## Git

A brief introduction to how Git is used with ShapeWorks can be found in the section, [How to Contribute to ShapeWorks?](contribute.md) 

Also highly recommended is the [Git How To](https://githowto.com).

## GitHub

The ShapeWorks Git repository is hosted on GitHub:

[https://github.com/SCIInstitute/ShapeWorks](https://github.com/SCIInstitute/ShapeWorks)

As described in the section about Git, the ShapeWorks team uses Pull Requests (PR) with peer review to integrate changes.  

Pull Requests with new features should include automated tests using the [Google Test framework](https://google.github.io/googletest/).  Please look at existing tests for examples.

## Coding Style Guide

We adopt the [Google Style Guide](https://google.github.io/styleguide/cppguide.html) for C++ coding style.

## C++ Core Guidelines

A highly recommended resource for C++ are the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).

## Building ShapeWorks

Please see: [How to Build ShapeWorks from Source?](build.md)

## Continuous Integration / Continuous Delivery

Each pull request triggers builds on Windows, Mac, and Linux runners using GitHub Actions.  More about GitHub Actions with ShapeWorks can be found [here](gh-actions.md).

Build artifacts are created for each build and kept automatically for a period of time.  These are full binary installers suitable for end-user install.