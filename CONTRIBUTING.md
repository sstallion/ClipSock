# Contributing

If you have an idea or feature request, please open an [issue][1] even if you do
not have time to contribute!

## Making Changes

> **Note**: This guide assumes you have a working Windows installation with the
> latest [Windows SDK][2] installed. [CMake 3.26][3] (or newer) is required to
> build the project using either [Visual Studio 2019][4] or [Clang 16][5].

To get started, [fork][6] this repository on GitHub and clone a working copy for
development:
```
git clone git@github.com:YOUR-USERNAME/ClipSock.git
```

To build the Debug configuration using Visual Studio, issue:
```
cmake -B build && cmake --build build
```

> **Note**: Once built, the ClipSock executable can be found in the build
> directory.

To run tests, issue:
```
ctest --test-dir build
```

Finally, commit changes and create a [pull request][7] against the default
branch for review. At a minimum, there should be no test regressions and
additional tests should be added for new functionality.

## Making Releases

Making releases is automated by [GitHub Actions][8]. Releases are created from
the default branch; as such, tests should be passing at all times.

To make a release, perform the following:

1. Create a new section in [CHANGELOG.md][9] for the release, and move items
   from Unreleased to this section. Links should be updated to point to the
   correct tags for comparison.

2. To commit outstanding changes, issue:
   ```
   git commit -a -m "Release v<version>"
   ```

3. Push changes to the remote repository and verify the results of the [CI][10]
   workflow:
   ```
   git push origin <default-branch>
   ```

4. To create a release tag, issue:
   ```
   git tag -a -m "Release v<version>" v<version>
   ```

5. Push the release tag to the remote repository and verify the results of the
   [Release][11] workflow:
   ```
   git push origin --tags
   ```

6. Review the draft release created by GitHub Actions and make adjustments as
   needed. Once finished, publish the release to make it publicly available.

## License

By contributing to this repository, you agree that your contributions will be
licensed under its Simplified BSD License.

[1]: https://github.com/sstallion/ClipSock/issues
[2]: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
[3]: https://cmake.org/
[4]: https://visualstudio.microsoft.com/
[5]: https://clang.llvm.org/
[6]: https://docs.github.com/en/github/getting-started-with-github/fork-a-repo
[7]: https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request
[8]: https://docs.github.com/en/actions
[9]: https://github.com/sstallion/ClipSock/blob/master/CHANGELOG.md
[10]: https://github.com/sstallion/ClipSock/actions/workflows/ci.yml
[11]: https://github.com/sstallion/ClipSock/actions/workflows/release.yml
