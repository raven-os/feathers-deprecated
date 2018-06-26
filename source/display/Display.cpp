#include "display/Display.hpp"

namespace display
{
  void Subcompositor::render(WindowTree const &windowTree)
  {
    // get next image data, and image to present
    auto [index, frame] = displaySystem.getImage(imageAvailable);


    magma::Semaphore<claws::no_delete> renderDone(displaySystem.userData.render(device, windowTree, index, displaySystem.swapchainUserData, frame, imageAvailable));

    //std::cout << "about to present for index " << index << std::endl;
    displaySystem.presentImage(renderDone, index); // present our image
  }
}
