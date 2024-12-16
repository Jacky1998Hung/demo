#!/bin/bash
cd build/
opt --passes=dot-cfg -disable-output output.ll

mv .main.dot rotate2.dot

mv rotate2.dot ../../../demo/
