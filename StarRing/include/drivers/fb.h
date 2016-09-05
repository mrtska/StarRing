
#pragma once



#include <pci.h>
#include <list.h>







class frame_buffer {

private:

	static class list<class frame_buffer> frame_buffer_list;


protected:



	struct pci_device *pci_device;


	int width;
	int height;

	void *vram;






public:

	class list<class frame_buffer> list;

	virtual bool verify_pci_id(struct pci_device *pci_device) = 0;

	virtual void frame_buffer_init(struct pci_device *pci_device) = 0;


	static class list<class frame_buffer> *get_frame_buffer_list() {

		return &frame_buffer_list;
	}




	frame_buffer() {

		frame_buffer_list.add_list_tail(&this->list);
	}

	virtual ~frame_buffer() {}





};















