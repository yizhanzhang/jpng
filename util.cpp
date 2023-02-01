bool endsWith(std::string const &str, std::string const &suffix) {
  if (str.length() < suffix.length()) {
      return false;
  }
  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}