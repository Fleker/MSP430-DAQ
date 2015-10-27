int i;
void setup()
{
  Serial.begin(4800);
  Serial.println("--START--");
  i = 0;
}

void loop()
{
  //Execute 30 loops
  if(i < 30) {
    Serial.print(i);
    Serial.println(",0");
    delay(500);
    i++;
  }
  if(i >= 30)
    Serial.println("--FINISHED--");
    delay(250);
}
