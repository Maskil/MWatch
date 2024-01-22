#include "mwatch.h"
#include "settings.h"

MWatch watch(settings); //instantiate your watchface

void setup() {
  watch.init();
}

void loop() {}
