import numpy as np

# Hoping I didnt get this wrong due to the axes in Skyrim defining Z as upward and Y as forward
def euler_to_rotation_matrix(yaw, pitch, roll):
    yaw = np.radians(yaw)
    pitch = np.radians(pitch)
    roll = np.radians(roll)
    Rz = np.array([
        [np.cos(yaw), -np.sin(yaw), 0],
        [np.sin(yaw), np.cos(yaw), 0],
        [0, 0, 1]
    ])
    Rx = np.array([
        [1, 0, 0],
        [0, np.cos(pitch), -np.sin(pitch)],
        [0, np.sin(pitch), np.cos(pitch)]
    ])
    Ry = np.array([
        [np.cos(roll), 0, np.sin(roll)],
        [0, 1, 0],
        [-np.sin(roll), 0, np.cos(roll)]
    ])
    R = Rz @ Rx @ Ry
    return R

R_initial = euler_to_rotation_matrix(-158.18, -1.51, -54.54)
R_desired = euler_to_rotation_matrix(0, 0, 90)

R_initial_inv = np.linalg.inv(R_initial)
T = R_desired @ R_initial_inv

print(T)
