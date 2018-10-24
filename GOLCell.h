#ifndef CELL_H
#define CELL_H

// A simple container class for game of life cells


class Cell{
  private:
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    bool alive;
  public:
    Cell(){
      xmin = 0;
      xmax = 0;
      ymin = 0;
      ymax = 0;
      alive = 0;
    }
    Cell(int x1, int x2, int y1, int y2){
      xmin = x1;
      xmax = x2;
      ymin = y1;
      ymax = y2;
      alive = 0;
    }

  void setXmin(int x){
    xmin = x;
  }
  void setXmax(int x){
    xmax = x;
  }
  void setYmin(int y){
    ymin = y;
  }
  void setYmax(int y){
    ymax = y;
  }
  void setAlive(bool b_alive){
    alive = b_alive;
  }
  // for brevity, only the mins have getters; max can easily be found from min+size
  int getXmin(){
    return xmin;
  }
  int getYmin(){
    return ymin;
  }
  int getXmax(){
    return xmax;
  }
  int getYmax(){
    return ymax;
  }
  bool isAlive(){
    return alive;
  }

  void Update(int X, int Y){
    if(X > xmin && X < xmax && Y > ymin && Y < ymax){
      alive = !alive; 
    }
  }
};
#endif
