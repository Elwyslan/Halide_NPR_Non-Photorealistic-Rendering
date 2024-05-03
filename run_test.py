import numpy as np
import cv2

def test_1():
    target = cv2.imread("reference_outputs/animal.png")[..., ::-1]
    hl_out = cv2.imread("outputs/animal.png")[..., ::-1]
    assert np.mean(np.abs(hl_out - target)) <= 15, "Mean Absolute Error between target and halide output bigger than threshold (15)."

def test_2():
    target = cv2.imread("reference_outputs/bald_eagle_portrait.png")[..., ::-1]
    hl_out = cv2.imread("outputs/bald_eagle_portrait.png")[..., ::-1]
    assert np.mean(np.abs(hl_out - target)) <= 15, "Mean Absolute Error between target and halide output bigger than threshold (15)."

def test_3():
    target = cv2.imread("reference_outputs/camera.png")[..., ::-1]
    hl_out = cv2.imread("outputs/camera.png")[..., ::-1]
    assert np.mean(np.abs(hl_out - target)) <= 15, "Mean Absolute Error between target and halide output bigger than threshold (15)."

def test_4():
    target = cv2.imread("reference_outputs/mandrill.png")[..., ::-1]
    hl_out = cv2.imread("outputs/mandrill.png")[..., ::-1]
    assert np.mean(np.abs(hl_out - target)) <= 15, "Mean Absolute Error between target and halide output bigger than threshold (15)."

def test_5():
    target = cv2.imread("reference_outputs/monarch.png")[..., ::-1]
    hl_out = cv2.imread("outputs/monarch.png")[..., ::-1]
    assert np.mean(np.abs(hl_out - target)) <= 15, "Mean Absolute Error between target and halide output bigger than threshold (15)."

def test_6():
    target = cv2.imread("reference_outputs/moon.png")[..., ::-1]
    hl_out = cv2.imread("outputs/moon.png")[..., ::-1]
    assert np.mean(np.abs(hl_out - target)) <= 15, "Mean Absolute Error between target and halide output bigger than threshold (15)."

def test_7():
    target = cv2.imread("reference_outputs/peppers3.png")[..., ::-1]
    hl_out = cv2.imread("outputs/peppers3.png")[..., ::-1]
    assert np.mean(np.abs(hl_out - target)) <= 15, "Mean Absolute Error between target and halide output bigger than threshold (15)."
