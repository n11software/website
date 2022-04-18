import Link from 'next/link'

let ContactButton = props => {
  return <Link href="/contact">
    <a className="flex cursor-pointer space-x-2 justify-center items-center py-2 px-2 rounded-lg transition-colors active:bg-black active:text-gray-200 text-gray-800 hover:bg-stone-300">
      Contact
    </a>
  </Link>
}
export default ContactButton