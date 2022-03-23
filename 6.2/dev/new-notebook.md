# How to Add a New Notebook?

Adding a new use case entails the following.

- Creating a notebook
- Notebook documentation 

## Creating a notebook

To add a new notebook to the codebase: 

- Add `<notebook-name>.ipynb` to `Examples/Python/tutorials/notebooks/`, where the `<notebook-name>` is a descriptive name for what is demonstrated in the notebook.
- Include links to other notebooks explaining how to import shapeworks and define visualization functions. 
- Before checking in a notebook in clear all output by clicking `Kernel -> Restart & Clear Output`.

## Notebook documentation

Screenshots and videos of all notebook output should be added so that a user can view notebook output without running the notebook.

### Adding a video to a notebook

1. Capture a screen recording in the format '.gif' or '.mp4'.
   - To screen record on Mac: Use `Shift + cmd + 5` to begin recording. This will generate a '.mov' video which can be converted to '.mp4' using the command line tool `FFmpeg`.
   - To screen record on Linux: The open source software 'XVidCap Screen Capture' can be downloaded and used to record screen videos. The video can then be saved in any format.
   - To screen record on Windows: Use `Windows key + Alt + R` to begin recording. The video format can be converted using the command line tool `FFmpeg`.
2. Copy the image or video to `/usr/sci/www/shapeworks/doc-resources/`. This requires access to the 'shapeworks' group which SCI support can grant. 
3. Add a markdown cell to the notebook that includes the video source. For example:
    ```
    <p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/example-video.mp4" autoplay muted loop controls style="width:100%"></p>
    ```
Once all output is included as an image or video in the notebook, add a copy of the notebook to `/ShapeWorks/docs/notebooks/`. Then update `/ShapeWorks/mkdocs.yml` to include the new notebook. 
