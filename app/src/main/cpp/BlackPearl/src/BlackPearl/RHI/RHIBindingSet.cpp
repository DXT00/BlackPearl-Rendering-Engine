#include "pch.h"
#include "RHIBindingSet.h"
namespace  BlackPearl{
    TextureSubresourceSet BindingSetItem::AllSubresources = TextureSubresourceSet(0, TextureSubresourceSet::AllMipLevels, 0, TextureSubresourceSet::AllArraySlices);
}
