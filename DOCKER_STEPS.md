# Docker Commands Used (Step by Step)

This document captures the common Docker command flow for this project.

## 1) Build the Docker image

```powershell
docker build -t multithreading-proj:latest .
```

What this does:
- Builds the image using the `Dockerfile` in this folder.
- Runs CMake build + test steps inside the build stage.
- Produces a runtime image that starts `scheduler_demo`.

## 2) Run the container

```powershell
docker run --rm --name multithreading-proj-run multithreading-proj:latest
```

What this does:
- Starts the app (`/app/scheduler_demo`) from the runtime image.
- `--rm` removes the container automatically after it exits.

## 3) (Optional) View images

```powershell
docker images
```

What this does:
- Lists local Docker images and confirms the image exists.

## 4) (Optional) View all containers

```powershell
docker ps -a
```

What this does:
- Shows running and exited containers.

## 5) (Optional) Remove image when done

```powershell
docker rmi multithreading-proj:latest
```

What this does:
- Deletes the local image to free disk space.

## Quick one-line flow

```powershell
docker build -t multithreading-proj:latest .
docker run --rm --name multithreading-proj-run multithreading-proj:latest
```

## Notes

- If your image name/tag was different, replace `multithreading-proj:latest` with your actual value.
- If you want me to document the exact command history you used, run the command again in the terminal and I can capture it precisely.
