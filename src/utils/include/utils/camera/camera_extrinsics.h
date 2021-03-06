/*
 * Copyright (c) 2015, The Regents of the University of California (Regents).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *    3. Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Please contact the author(s) of this library if you have any questions.
 * Authors: Erik Nelson            ( eanelson@eecs.berkeley.edu )
 *          David Fridovich-Keil   ( dfk@eecs.berkeley.edu )
 */

///////////////////////////////////////////////////////////////////////////////
//
// This class defines a camera's extrinsic parameters according to the OpenCV
// camera model:
// http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html
//
// The default camera frame is the same as the default world frame.
//
//        +Z
//         ^
//         |
//         |
//         |
//         |
//       World -------->+Y
//       /
//      /
//     /
//    /
//   v
//   +X
//
///////////////////////////////////////////////////////////////////////////////

#ifndef BSFM_CAMERA_CAMERA_EXTRINSICS_H
#define BSFM_CAMERA_CAMERA_EXTRINSICS_H

#include <utils/math/transform_3d.h>
#include <Eigen/Core>

namespace bsfm {

using Eigen::Matrix3d;
using Eigen::Matrix4d;
using Eigen::Vector3d;

class CameraExtrinsics {

public:
  // Constructor. Initialize to identity.
  CameraExtrinsics();

  // Constructor. Initialize world_to_camera_.
  CameraExtrinsics(const Transform3D& world_to_camera);

  // Set world_to_camera_.
  void SetWorldToCamera(const Transform3D& world_to_camera);

  // Extract poses.
  Transform3D WorldToCamera() const;
  Transform3D CameraToWorld() const;

  // Rotate the world-to-camera frame.
  void SetRotation(const Matrix3d& rotation);
  void SetRotation(double phi, double theta, double psi);
  void Rotate(const Matrix3d& delta);
  void Rotate(double dphi, double dtheta, double dpsi);
  Matrix3d Rotation() const;

  // Translate the world-to-camera frame. All inputs correspond to the
  // coordinates of the camera in world-frame.
  void SetTranslation(const Vector3d& translation);
  void SetTranslation(double wx, double wy, double wz);
  void Translate(const Vector3d& delta);
  void Translate(double dx, double dy, double dz);
  void TranslateX(double dx);
  void TranslateY(double dy);
  void TranslateZ(double dz);
  Vector3d Translation() const;

  // The extrinsics matrix is 3x4 matrix: [R | t].
  Matrix34d Rt() const;

  // Convert a world frame point into the camera frame.
  void WorldToCamera(double wx, double wy, double wz,
                     double* cx, double* cy, double* cz) const;

  // Convert a camera frame point into the world frame.
  void CameraToWorld(double cx, double cy, double cz,
                     double* wx, double* wy, double* wz) const;

 private:
  Transform3D world_to_camera_;

};  //\class CameraExtrinsics

// ---------------------------------- IMPLEMENTATION ----------------------------

 // Constructor. Initialize to identity.
 CameraExtrinsics::CameraExtrinsics() {
   world_to_camera_ = Transform3D();
 }

 // Constructor. Initialize world_to_camera_ .
 CameraExtrinsics::CameraExtrinsics(const Transform3D& world_to_camera)
   : world_to_camera_(world_to_camera) {}

 // Set world_to_camera_.
 void CameraExtrinsics::SetWorldToCamera(const Transform3D& world_to_camera) {
   world_to_camera_ = world_to_camera;
 }

 // Extract poses.
 Transform3D CameraExtrinsics::WorldToCamera() const {
   return world_to_camera_;
 }

 Transform3D CameraExtrinsics::CameraToWorld() const {
   return world_to_camera_.Inverse();
 }

 // For use in the following methods: From H&Z page 156, the extrinsics matrix
 // can be represented as
 //   [R -Rc]
 //   [0  1 ]
 // where c is the camera centroid. From this we get t = -Rc and c = -R't
 void CameraExtrinsics::SetRotation(const Matrix3d& rotation) {
   const Vector3d t = world_to_camera_.GetTranslation();
   const Matrix3d R = world_to_camera_.GetRotation();
   const Vector3d c = -R.transpose() * t;

   world_to_camera_.SetRotation(rotation);
   world_to_camera_.SetTranslation(-rotation * c);
 }

 void CameraExtrinsics::SetRotation(double phi, double theta, double psi) {
   SetRotation(EulerAnglesToMatrix(phi, theta, psi));
 }

 void CameraExtrinsics::Rotate(const Matrix3d& delta) {
   const Matrix3d R = world_to_camera_.GetRotation();
   SetRotation(delta * R);
 }

 void CameraExtrinsics::Rotate(double dphi, double dtheta, double dpsi) {
   Rotate(EulerAnglesToMatrix(dphi, dtheta, dpsi));
 }

 Matrix3d CameraExtrinsics::Rotation() const {
   return world_to_camera_.GetRotation();
 }

 void CameraExtrinsics::SetTranslation(const Vector3d& translation) {
   const Matrix3d R = world_to_camera_.GetRotation();
   world_to_camera_.SetTranslation(-R * translation);
 }

 void CameraExtrinsics::SetTranslation(double x, double y, double z) {
   SetTranslation(Vector3d(x, y, z));
 }

 void CameraExtrinsics::Translate(const Vector3d& delta) {
   const Vector3d t = world_to_camera_.GetTranslation();
   const Matrix3d R = world_to_camera_.GetRotation();
   Vector3d c = -R.transpose() * t;

   c += delta;
   world_to_camera_.SetTranslation(-R*c);
 }

 void CameraExtrinsics::Translate(double dx, double dy, double dz) {
   Translate(Vector3d(dx, dy, dz));
 }

 void CameraExtrinsics::TranslateX(double dx) {
   Translate(Vector3d(dx, 0, 0));
 }

 void CameraExtrinsics::TranslateY(double dy) {
   Translate(Vector3d(0, dy, 0));
 }

 void CameraExtrinsics::TranslateZ(double dz) {
   Translate(Vector3d(0, 0, dz));
 }

 Vector3d CameraExtrinsics::Translation() const {
   const Vector3d t = world_to_camera_.GetTranslation();
   const Matrix3d R = world_to_camera_.GetRotation();
   return -R.transpose() * t;
 }

 // The extrinsics matrix is 3x4 matrix: [R | t].
 Matrix34d CameraExtrinsics::Rt() const {
   return world_to_camera_.Dehomogenize();
 }

 // Convert a world frame point into the camera frame.
 void CameraExtrinsics::WorldToCamera(double wx, double wy, double wz,
                                      double* cx, double* cy, double* cz) const {
   CHECK_NOTNULL(cx);
   CHECK_NOTNULL(cy);
   CHECK_NOTNULL(cz);

   const Vector3d w(wx, wy, wz);
   const Vector3d c = world_to_camera_ * w;

   *cx = c(0);
   *cy = c(1);
   *cz = c(2);
 }

 // Convert a camera frame point into the world frame.
 void CameraExtrinsics::CameraToWorld(double cx, double cy, double cz,
                                      double* wx, double* wy, double* wz) const {
   CHECK_NOTNULL(wx);
   CHECK_NOTNULL(wy);
   CHECK_NOTNULL(wz);

   const Vector3d c(cx, cy, cz);
   const Vector3d w = CameraExtrinsics::CameraToWorld() * c;

   *wx = w(0);
   *wy = w(1);
   *wz = w(2);
 }

}  //\namespace bsfm

#endif
