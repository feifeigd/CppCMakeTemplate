#pragma once

class noncopyable
{
protected:
  noncopyable() {}
private:
  noncopyable(const noncopyable&);
  const noncopyable& operator=(const noncopyable&);
};
