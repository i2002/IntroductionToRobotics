// // char buf[16] = "";
// // int step = 0;
// int steps = 14;
// int points = 0;

// void preview(byte step) {
//   // Serial.println(step);
//   switch (step) {
//     case 0:
//       gameDisp.displayImage(startGameImage);
//       break;

//     case 1:
//       gameDisp.displayImage(settingsImage);
//       break;
//   }
// }

// void apply(byte step) {
//   // Serial.print("Selected value: ");
//   Serial.println(step);
//   inputManager.setupRangeInput("Brightness", brightnessPreview, brightnessAction);
// }

// void brightnessPreview(byte step) {
//   analogWrite(pinA, step * (256 / RangeInput::maxSteps));
// }

// void brightnessAction(byte step) {
//   // Serial.print("Selected value: ");
//   Serial.println(step * (256 / RangeInput::maxSteps));
// }

// const char* list[] = {
//   "Test",
//   "Second"
// };

// const char * label(byte id) {
//   return list[id];
// }