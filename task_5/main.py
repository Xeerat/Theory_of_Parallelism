import os

os.environ["OPENCV_LOG_LEVEL"] = "SILENT"
os.environ["OPENCV_VIDEOIO_PRIORITY_FFMPEG"] = "0"
os.environ["OPENCV_VIDEOIO_DEBUG"] = "0"

import argparse
import time
import cv2
import torch

from ultralytics import YOLO
from multiprocessing import Process, Queue, cpu_count
from queue import Empty


class VideoCaptureRAII:
    def __init__(self, source):
        self.cap = cv2.VideoCapture(source)

        if not self.cap.isOpened():
            raise RuntimeError("Cannot open video source")

    def read(self):
        return self.cap.read()

    def get(self, prop):
        return self.cap.get(prop)

    def release(self):
        if self.cap.isOpened():
            self.cap.release()

    def __del__(self):
        self.release()


class VideoWriterRAII:
    def __init__(self, path, fps, size):
        fourcc = cv2.VideoWriter_fourcc(*"mp4v")
        self.writer = cv2.VideoWriter(path, fourcc, fps, size)

    def write(self, frame):
        self.writer.write(frame)

    def release(self):
        self.writer.release()

    def __del__(self):
        self.release()


class YOLOModel:
    def __init__(self):
        self.model = YOLO("yolov8s-pose.pt")

    def infer(self, frame):
        return self.model(frame, verbose=False)


def process_frame(model, frame):
    results = model.infer(frame)
    return results[0].plot()


def worker(input_q, output_q):
    torch.set_num_threads(1)
    model = YOLOModel()

    while True:
        item = input_q.get()

        if item is None:
            break

        idx, frame = item
        processed = process_frame(model, frame)
        output_q.put((idx, processed))


def create_writer(cap, output_path):
    fps = cap.get(cv2.CAP_PROP_FPS)

    if fps <= 0:
        fps = 30

    width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    writer = VideoWriterRAII(output_path, fps, (width, height))

    return writer, fps


def run_single(source, output_path):
    try:
        cap = VideoCaptureRAII(source)
    except RuntimeError:
        print("Source not found")
        return 
    
    writer, _ = create_writer(cap, output_path)

    model = YOLOModel()

    frame_count = 0
    start = time.time()

    while True:
        ret, frame = cap.read()

        if not ret:
            print("Camera read error")
            break

        frame = process_frame(model, frame)

        if source == 0:
            cv2.imshow("frame", frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        writer.write(frame)
        frame_count += 1

    end = time.time()
    elapsed = end - start

    cap.release()
    writer.release()

    cv2.destroyAllWindows()

    print(f"{elapsed:.4f}")


def run_multi(source, output_path, n_workers):
    try:
        cap = VideoCaptureRAII(source)
    except RuntimeError:
        print("Source not found")
        return 

    writer, _ = create_writer(cap, output_path)

    input_q = Queue(maxsize=n_workers * 2)
    output_q = Queue(maxsize=n_workers * 2)

    workers = []

    for _ in range(n_workers):
        p = Process(target=worker, args=(input_q, output_q))
        p.start()
        workers.append(p)

    frame_id = 0
    next_id = 0

    total_sent = 0
    total_received = 0

    frame_buffer = {}

    start = time.time()
    run = True

    while run:
        ret, frame = cap.read()

        if not ret:
            print("Camera read error")
            break

        input_q.put((frame_id, frame))

        total_sent += 1
        frame_id += 1

        while True:
            try:
                idx, processed = output_q.get_nowait()
                frame_buffer[idx] = processed
                total_received += 1

            except Empty:
                break

        while next_id in frame_buffer:
            frame = frame_buffer.pop(next_id)

            if source == 0:
                cv2.imshow("frame", frame)

                if cv2.waitKey(1) & 0xFF == ord('q'):
                    run = False
                    break

            writer.write(frame)
            next_id += 1

    for _ in workers:
        input_q.put(None)

    while total_received < total_sent:
        idx, processed = output_q.get()
        frame_buffer[idx] = processed
        total_received += 1

        while next_id in frame_buffer:
            frame = frame_buffer.pop(next_id)
            writer.write(frame)
            next_id += 1

    for p in workers:
        p.join()

    cap.release()
    writer.release()

    input_q.close()
    output_q.close()

    cv2.destroyAllWindows()

    end = time.time()

    elapsed = end - start

    print(f"{elapsed:.4f}")


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--video_path", default=None)
    parser.add_argument("--output", default="output.mp4")
    parser.add_argument("--mode", choices=["s", "m"], default="s")
    parser.add_argument("--workers", type=int, default=max(1, cpu_count() // 2))
    parser.add_argument("--camera", action="store_true")

    args = parser.parse_args()

    if args.camera:
        source = 0

    else:
        if args.video_path is None:
            raise ValueError("Provide video_path or use --camera")

        source = args.video_path

    if args.mode == "s":
        run_single(source, args.output)

    else:
        run_multi(source, args.output, args.workers)


if __name__ == "__main__":
    main()