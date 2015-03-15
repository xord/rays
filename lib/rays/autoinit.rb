# -*- coding: utf-8 -*-


require 'rays/ext'


unless $RAYS_NOAUTOINIT
  Rays.init!
  at_exit {Rays.fin!}
end
